#include "pch.h"
#include "XAudioLoader.h"

XAudioBuffer::XAudioBuffer(void* ptrData, size_t size, WAVEFORMATEXTENSIBLE wav) :
	m_ptrData(ptrData),
	m_size(size),
    m_wav(wav)
{}

XAudioBuffer::~XAudioBuffer() {
	if (m_ptrData) {
		free(m_ptrData);
	}
}

void* XAudioBuffer::ptr() {
	return m_ptrData;
}

WAVEFORMATEXTENSIBLE XAudioBuffer::wav() {
    return m_wav;
}

size_t XAudioBuffer::size() {
	return m_size;
}

XAudioBuffer XAudioLoader::loadFromFile(LPCWSTR file) {
    HRESULT hr;
    
    // Open File
    HANDLE hFile = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        GetLogger().log("Failed to open audio file!");
        return XAudioBuffer();
    }

    // Vars to work with
    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    DWORD filetype;

    // Find and read chunk
    if (FAILED(hr = findChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition))) {
        GetLogger().log("Audio loading failed at \"findChunk\" for \"fourccRIFF\"").log(hr);
        return XAudioBuffer();
    }
    if (FAILED(hr = readChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition))) {
        GetLogger().log("Audio loading failed at \"readChunkData\" for \"fourccRIFF\"").log(hr);
        return XAudioBuffer();
    }

    // Check if type match
    if (filetype != fourccWAVE) {
        GetLogger().log("Audio loading failed! Not a wav file!");
        return XAudioBuffer();
    }

    // Wave header
    WAVEFORMATEXTENSIBLE waveHeader;
    ZeroMemory(&waveHeader, sizeof(WAVEFORMATEXTENSIBLE));

    // Find format chunk and load data
    if (FAILED(hr = findChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition))) {
        GetLogger().log("Audio loading failed at \"findChunk\" for \"fourccFMT\"").log(hr);
        return XAudioBuffer();
    }
    if (FAILED(hr = readChunkData(hFile, &waveHeader, dwChunkSize, dwChunkPosition))) {
        GetLogger().log("Audio loading failed at \"readChunkData\" for \"fourccFMT\"").log(hr);
        return XAudioBuffer();
    }

    // Find data chunk
    if (FAILED(hr = findChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition))) {
        GetLogger().log("Audio loading failed at \"findChunk\" for \"fourccDATA\"").log(hr);
        return XAudioBuffer();
    }

    // Allocate memory for chunk
    void* ptrBuffer = malloc(dwChunkSize);
    
    // Read chunk content
    if (FAILED(hr = readChunkData(hFile, ptrBuffer, dwChunkSize, dwChunkPosition))) {
        GetLogger().log("Audio loading failed at \"readChunkData\" for \"fourccDATA\"").log(hr);
        return XAudioBuffer();
    }

    // Return buffer
    return XAudioBuffer(ptrBuffer, dwChunkSize, waveHeader);
}

HRESULT XAudioLoader::findChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition) {
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;
}

HRESULT XAudioLoader::readChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset) {
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}
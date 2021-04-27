#pragma once
#include "pch.h"

/*
* Part of this source file was copyed and / or inspired by: https://docs.microsoft.com/en-gb/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
* Copyright Microsoft Corporation / Windows Docs (CC BY 4.0 - https://creativecommons.org/licenses/by/4.0/deed.en)
*/

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

class XAudioBuffer {
	public:
		XAudioBuffer() = default;
		XAudioBuffer(void* ptrData, size_t size, WAVEFORMATEXTENSIBLE wav);
		~XAudioBuffer();

		void* ptr();
		WAVEFORMATEXTENSIBLE wav();
		size_t size();

	private:
		void* m_ptrData = nullptr;
		size_t m_size = 0;
		WAVEFORMATEXTENSIBLE m_wav = {};
};

class XAudioLoader {
	public:
		static XAudioBuffer loadFromFile(LPCWSTR file);

	private:
		static HRESULT findChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
		static HRESULT readChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
};

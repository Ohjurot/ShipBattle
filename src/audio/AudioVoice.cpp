#include "pch.h"
#include "AudioVoice.h"

AudioVoice::AudioVoice(IXAudio2* ptrAudio, LPCWSTR file) :
	m_buffer(XAudioLoader::loadFromFile(file))
{
	HRESULT hr;

	// Abort if invlid
	if (!m_buffer.ptr()) {
		return;
	}

	// Create voice
	auto wv = m_buffer.wav();
	if (FAILED(hr = ptrAudio->CreateSourceVoice(&m_ptrVoice, (WAVEFORMATEX*)&wv))) {
		GetLogger().log("ptrAudio->CreateSourceVoice(...) failed!").log(hr);
		return;
	}
}

AudioVoice::~AudioVoice(){
	if (m_ptrVoice)
	{
		m_ptrVoice->DestroyVoice();
	}
}

void AudioVoice::play(bool loop){	
	HRESULT hr;

	if (m_ptrVoice)
	{
        // Create buffer
        XAUDIO2_BUFFER buffer;
        ZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));

        // Default buffer desription
        buffer.AudioBytes = m_buffer.size();
        buffer.pAudioData = (BYTE*)m_buffer.ptr();
        buffer.Flags = XAUDIO2_END_OF_STREAM;

        // Buffer looping
        if (loop) {
            buffer.LoopBegin = 0;
            buffer.LoopLength = 0;
            buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
        }

        // Submit buffer and play
        if (FAILED(hr = m_ptrVoice->SubmitSourceBuffer(&buffer))) {
            GetLogger().log("m_ptrVoice->SubmitSourceBuffer(...) failed!").log(hr);
            return;
        }
        m_ptrVoice->Start();
	}
}

void AudioVoice::stop(){
	HRESULT hr;

	// stop
	if (m_ptrVoice)
	{
        if (FAILED(hr = m_ptrVoice->Stop())) {
            GetLogger().log("m_ptrVoice->Stop(...) failed!").log(hr);
            return;
        }
	}
}

void AudioVoice::setVolume(float vol) {
	HRESULT hr;

	if (m_ptrVoice)
	{
        // Set volume
        if (FAILED(hr = m_ptrVoice->SetVolume(vol))) {
            GetLogger().log("m_ptrVoice->SetVolume(...) failed!").log(hr);
            return;
        }
	}
}

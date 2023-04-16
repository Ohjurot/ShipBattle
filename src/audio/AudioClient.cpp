#include "pch.h"
#include "AudioClient.h"

AudioClient::AudioClient(){
	HRESULT hr;

	// Create XAudio2 instance
	if (FAILED(hr = XAudio2Create(m_xa2.to(), NULL, XAUDIO2_DEFAULT_PROCESSOR))) {
		GetLogger().log("XAudio2Create(...) failed!").log(hr);
		return;
	}

	// Create master voice
	if (FAILED(hr = m_xa2->CreateMasteringVoice(&m_masterVoice))) {
		GetLogger().log("m_xa2->CreateMasteringVoice(...) failed!").log(hr);
		return;
	}
}

AudioClient::~AudioClient(){
	// Destroy release
	m_masterVoice->DestroyVoice();
	m_xa2.release();
}

IXAudio2* AudioClient::getAudio(){
	return m_xa2;
}

void AudioClient::setVolume(float vol) {
	HRESULT hr;

	if (FAILED(hr = m_masterVoice->SetVolume(vol))) {
		GetLogger().log("m_masterVoice->SetVolume(...) failed!").log(hr);
		return;
	}
}

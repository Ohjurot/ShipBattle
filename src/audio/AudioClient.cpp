#include "pch.h"
#include "AudioClient.h"

AudioClient::AudioClient(){
	// Create XAudio2 instance
	if (FAILED(XAudio2Create(m_xa2.to(), NULL, XAUDIO2_DEFAULT_PROCESSOR))) {
		return;
	}

	// Create master voice
	m_xa2->CreateMasteringVoice(&m_masterVoice);
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
	m_masterVoice->SetVolume(vol);
}

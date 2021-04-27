#include "pch.h"
#include "AudioVoice.h"

AudioVoice::AudioVoice(IXAudio2* ptrAudio, LPCWSTR file) :
	m_buffer(XAudioLoader::loadFromFile(file))
{
	// Abort if invlid
	if (!m_buffer.ptr()) {
		return;
	}

	// Create voice
	auto wv = m_buffer.wav();
	if (FAILED(ptrAudio->CreateSourceVoice(&m_ptrVoice, (WAVEFORMATEX*)&wv))) {
		return;
	}
}

AudioVoice::~AudioVoice(){
	m_ptrVoice->DestroyVoice();
}

void AudioVoice::play(bool loop){	
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
	m_ptrVoice->SubmitSourceBuffer(&buffer);
	m_ptrVoice->Start();
}

void AudioVoice::stop(){
	// stop
	m_ptrVoice->Stop();
}

void AudioVoice::setVolume(float vol) {
	// Set volume
	m_ptrVoice->SetVolume(vol);
}

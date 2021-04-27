#pragma once
#include "pch.h"

#include <audio/XAudioLoader.h>

class AudioVoice {
	public:
		AudioVoice(IXAudio2* ptrAudio, LPCWSTR file);
		~AudioVoice();

		void play(bool loop = false);
		void stop();
		void setVolume(float vol);

	private:
		XAudioBuffer m_buffer;
		IXAudio2SourceVoice* m_ptrVoice = nullptr;
};
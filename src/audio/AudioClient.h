#pragma once
#include "pch.h"

class AudioClient {
	public:
		AudioClient();
		~AudioClient();

		IXAudio2* getAudio();
		void setVolume(float vol);

	private:
		ScopedComPointer<IXAudio2> m_xa2;
		IXAudio2MasteringVoice* m_masterVoice;
};
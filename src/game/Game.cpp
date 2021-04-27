#include "pch.h"
#include "Game.h"

Game::Game(AudioVoice** voiceArray) {
	// Set
	m_globalState.voiceArray = voiceArray;

	// Init states
	IGameState** ptrState = &m_states[0];
	while (*ptrState) {
		// Init state
		(*ptrState)->init(&m_globalState);
		ptrState++;
	}

	// Activate state zero
	if (m_states[0]) {
		m_states[0]->activate();
	}
}

void Game::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Get current state interface
	IGameState* ptrState = m_states[m_currentState];

	// Check state and call
	if (ptrState) {
		INT newState = ptrState->update(deltaTMs, refManager, refIOState);

		// Check if states differ
		if (newState >= 0 && newState != m_currentState) {
			// Leave old state
			ptrState->deactivate();

			// Load new state
			m_currentState = newState;
			if (m_states[m_currentState]) {
				m_states[m_currentState]->activate();
			}
		}
	}
}

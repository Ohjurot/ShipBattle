#pragma once
#include "pch.h"

#include <game/IGameState.h>
#include <game/IOState.h>

#include <rendering/EasyTex.h>
#include <rendering/QuadManger.h>

#include <game/states/MainMenue.h>
#include <game/states/PlaceFleet.h>
#include <game/states/PlayerAim.h>
#include <game/states/PlayerShooting.h>
#include <game/states/PlayerHit.h>
#include <game/states/PlayerNoHit.h>
#include <game/states/PlayerWin.h>
#include <game/states/PlayerLost.h>
#include <game/states/ComputerAim.h>
#include <game/states/ComputerShooting.h>
#include <game/states/ComputerNoHit.h>
#include <game/states/ComputerHit.h>

class Game {
	public:
		Game(AudioVoice** voiceArray);
		void update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState);

	private:
		GState_MainMenue state0;
		GState_PlaceFleet state1;
		GState_PlayerAim state2;
		GState_PlayerShooting state3;
		GState_PlayerNoHit state4;
		GState_PlayerHit state5;
		GState_ComputerAim state6;
		GState_ComputerShooting state7;
		GState_ComputerNoHit state8;
		GState_ComputerHit state9;
		GState_PlayerWin state10;
		GState_PlayerLost state11;

	private:
		GlobalState m_globalState = {};

		INT m_currentState = 0;
		IGameState* m_states[13] = {
			// State 0: Main Menue
			&state0,
			// State 1: Place Fleet
			&state1,
			// State 2: Player Aim
			&state2,
			// State 3: Player Shooting
			&state3,
			// State 4: Player no hit
			&state4,
			// State 5: Player hit
			&state5,
			// State 6: Computer aim
			&state6,
			// State 7: Computer shooting
			&state7,
			// State 8: Computer no hit
			&state8,
			// State 9: Computer hit
			&state9,
			// State 10: Game Win
			&state10,
			// State 11: Game lost
			&state11,
			// ARRAY TERMINATOR LEAVE IT!
			nullptr,
		};
};
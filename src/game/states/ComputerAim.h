#pragma once

#include <game/IGameState.h>
#include <game/tools/GeneralHelpers.h>
#include <game/tools/ShipFunctions.h>

#include <game/ai/GameAI.h>

class GState_ComputerAim : public IGameState {
	public:
		void init(GlobalState* ptrGlobalState) override;
		void activate() override;
		void deactivate() override;
		INT update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) override;

	private:
		GlobalState* m_ptrGlobalState;
		UINT64 m_elapsedTime;
};
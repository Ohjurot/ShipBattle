#pragma once

#include <game/IGameState.h>
#include <game/tools/GeneralHelpers.h>
#include <game/tools/ShipFunctions.h>

class GState_PlayerWin : public IGameState {
	public:
		void init(GlobalState* ptrGlobalState) override;
		void activate() override;
		void deactivate() override;
		INT update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) override;

	private:
		GlobalState* m_ptrGlobalState;
		bool m_debounce = false;
		bool m_showOwnShips = false;
};
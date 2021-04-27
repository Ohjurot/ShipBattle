#pragma once

#include <game/IGameState.h>
#include <game/tools/ShipFunctions.h>

class GState_PlaceFleet : public IGameState {
	public:
		void init(GlobalState* ptrGlobalState) override;
		void activate() override;
		void deactivate() override;
		INT update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) override;

	private:
		GlobalState* m_ptrGlobalState;
		int m_pickedShipIdx = -1;
		Ship m_pickShip;
		bool m_debounce = false;
		bool m_debounceKey = false;
};

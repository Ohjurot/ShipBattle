#pragma once

#include <game/IGameState.h>
#include <game/tools/GeneralHelpers.h>

class GState_MainMenue : public IGameState {
	public:
		void init(GlobalState* ptrGlobalState) override;
		void activate() override;
		void deactivate() override;
		INT update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) override;
};
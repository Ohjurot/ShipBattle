#pragma once
#include "pch.h"

#include <rendering/EasyTex.h>
#include <rendering/QuadManger.h>

#include <game/IOState.h>
#include <game/GlobalState.h>

class IGameState {
	public:
		virtual void init(GlobalState* ptrGlobalState) = 0;
		virtual void activate() = 0;
		virtual void deactivate() = 0;
		virtual INT update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) = 0;
};
#pragma once
#include "pch.h"

#include <game/GlobalState.h>

class GameAI {
	public:
		static void aiPlaceShot(GlobalState* ptrState, UINT* ptrTargetPX, UINT* ptrTargetPY);

	private:
		static void aiWeightOpertunitys(GlobalState* ptrState, POINT* arrayOportunitys, float* arrayWeights, unsigned int opertunitysCount);
		static void aiWeightCompare(float* arrWeights, unsigned int count, unsigned int* ptrCount, float* ptrMax);
		static float aiWeightPoint(GlobalState* ptrState, UINT px, UINT py, UINT r);
		static float aiWeightDirection(GlobalState* ptrState, UINT px, UINT py, UINT r, INT xd, INT yd);
		static unsigned int aiKillShips(GlobalState* ptrState, POINT* pointArray, unsigned int size);
		static void updateAiMap(GlobalState* ptrState);
		static unsigned int aiPointDistance(GlobalState* ptrState, INT x, INT y, INT ix, INT iy);
};
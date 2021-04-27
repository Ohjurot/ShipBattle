#pragma once
#include "pch.h"

#include <rendering/EasyTex.h>
#include <rendering/QuadManger.h>

#include <game/GlobalState.h>
#include <game/tools/GeneralHelpers.h>

class ShipFunctions {
	public:
		static bool shipCheckBounds(Ship* ptrShip);
		static bool shipCheckCollision(Ship* arrShips, unsigned int count, unsigned int index);
		static bool shipCheckCollision(Ship* arrShips, Ship* checker, unsigned int count, unsigned int index);
		static void randomShips(Ship* arrShips, unsigned int count);
		static void drawShip(Ship* ship, QuadManger& manager);
		static void drawShips(Ship* arrShips, unsigned int count, QuadManger& manager);
};
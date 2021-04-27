#pragma once
#include "pch.h"

#include <audio/AudioVoice.h>

struct Ship {
	UINT8 posx, posy, length;
	BOOL isVertical, alternate, visible;

	UINT8 hp;
};

enum class GameAIOportunity : UINT8 {
	RANDOM_PLACE = 0,

	HIT_SHIP_ALIVE = 1,

	SHIP_IMPOSSIBLE = 255,
};

struct GlobalState {
	UINT8 playfieldComputer[10][10];
	UINT8 playfieldPlayer[10][10];

	GameAIOportunity aiPlayField[10][10];
	UINT8 aiPossibleHits;

	Ship playerShips[5];
	Ship computerShips[5];

	INT lastShotShipIdx = -1;
	UINT8 aimX, aimY;

	AudioVoice** voiceArray;
};

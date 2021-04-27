#pragma once
#include "pch.h"

#include <rendering/QuadManger.h>
#include <rendering/EasyTex.h>

#define DRAWPLANE_SHIP		20
#define GRID_ORIGINE_LEFT	762
#define GRID_ORIGINE_TOP	183
#define GRID_SHIP_SCALE		0.6f
#define GRID_STEP_RATE		83

#define GENNERATE_CURSOR() EasyTex::textureAt(ET_TEXTURE_CURSOR, std::max<LONG>(0, refIOState.mouseX - 20L), std::max<LONG>(0, refIOState.mouseY - 50L), 100, 0.5f)
#define GENNERATE_INFO_BOX(texture) EasyTex::textureAt(texture, 365, 600, 15, 0.7f);
#define GENNERATE_RADAR(texture) EasyTex::textureAt(texture, 390, 210, 10, 1.2f);

enum class MouseClickArea : unsigned int {
	NONE = 0,
	RADAR = 1,
	INFOBOX = 2,
	PLAYFIELD = 3,
};

struct MouseClickBounds {
	MouseClickArea area;
	UINT tx, ty;
};

class GeneralHelpers{
	public:
		static MouseClickBounds evaluateMouse(UINT x, UINT y);
		static void drawHitMarkers(UINT8 area[10][10], QuadManger& refManager);
};

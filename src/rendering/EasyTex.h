#pragma once

#include <rendering/Vertex.h>

#define ET_TEXTURE_SHIP_2				0
#define ET_TEXTURE_SHIP_3				1
#define ET_TEXTURE_SHIP_4_1				2
#define ET_TEXTURE_SHIP_4_2				3
#define ET_TEXTURE_SHIP_5				4
#define ET_TEXTURE_HIT					5
#define ET_TEXTURE_NO_HIT				6
#define ET_TEXTURE_CROSSHAIR			7
#define ET_TEXTURE_RADAR_OWN			8
#define ET_TEXTURE_RADAR_ENEMY			9
#define ET_TEXTURE_CURSOR				10
#define ET_TEXTURE_BACKGROUND_MAIN		11
#define ET_TEXTURE_BACKGROUND_GAME		12
#define ET_TEXTURE_STAT_GAME_LOST		13
#define ET_TEXTURE_STAT_GAME_WIN		14
#define ET_TEXTURE_STAT_HIT				15
#define ET_TEXTURE_STAT_NO_HIT			16
#define ET_TEXTURE_STAT_PLACE_FLEET		17
#define ET_TEXTURE_STAT_PLACE_SHOT		18
#define ET_TEXTURE_STAT_SHOT_TRAVEL		19
#define ET_TEXTURE_STAT_WAIT			20
#define ET_TEXTURE_SHIP_ROT_2			21
#define ET_TEXTURE_SHIP_ROT_3			22
#define ET_TEXTURE_SHIP_ROT_4_1			23
#define ET_TEXTURE_SHIP_ROT_4_2			24
#define ET_TEXTURE_SHIP_ROT_5			25

struct TextureMeta {
	UINT32 texIndex;
	UINT32 pixelWidth, pixelHeight;
	float uv_left, uv_top, uv_bottom, uv_right;
};

class EasyTex {
	public:
		static TextureQuad textureAt(UINT texture, UINT x, UINT y, UINT8 layer = 10, float scale = 1.0f);

	private:
		static TextureMeta s_metaArray[26];
};
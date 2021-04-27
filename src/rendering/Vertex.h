#pragma once
#include "pch.h"

struct TextureQueadVertex {
	float pos[3];
	float uv[2];
	uint32_t texid;
};

struct TextureQuad {
	float top, left, bottom, right;
	float uv_top, uv_left, uv_bottom, uv_right;
	uint32_t texid;
	uint8_t layer;
};

// Textured quad shader. Compiled at startup (src/gfx/GShaderCompiler.cpp) into
// ImageQuadVS.cso (vsMain) and ImageQuadPS.cso (psMain).
// All resources (textures, sampler) come from the shared binding contract in Bindings.slang
// and are accessed through its Pipeline:: namespace.

import Bindings;

// === Types ===

// Must match the input layout in src/rendering/RenderState.cpp.
// (Slang rejects SV_POSITION as a vertex shader input, hence POSITION.)
struct VS_INPUT {
	float3 pos : POSITION;
	float2 uv : TEXTCORDS;
	uint texid : TEXID;
};

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float2 uv : TEXTCORDS;
	uint texid : TEXID;
};

struct PS_OUT {
	float4 color : SV_TARGET;
	float depth : SV_DEPTH;
};

// === Entry points ===

[shader("vertex")]
PS_INPUT vsMain(VS_INPUT vtx) {
	PS_INPUT ps;
	ps.pos = float4(vtx.pos, 1.0f);
	ps.uv = vtx.uv;
	ps.texid = vtx.texid;
	return ps;
}

[shader("pixel")]
PS_OUT psMain(PS_INPUT vdin) {
	PS_OUT psout;
	psout.color = Pipeline::textures[vdin.texid].Sample(Pipeline::texSampler, vdin.uv);
	psout.depth = psout.color.a > 0.5f ? vdin.pos.z : 0.0f;
	return psout;
}

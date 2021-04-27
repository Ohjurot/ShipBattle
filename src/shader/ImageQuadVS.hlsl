#include "RootSignature.hlsl"

struct VS_INPUT {
	float3 pos : SV_POSITION;
	float2 uv : TEXTCORDS;
	uint texid : TEXID;
};

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float2 uv : TEXTCORDS;
	uint texid : TEXID;
};

[RootSignature(ROOTSIG)]
PS_INPUT main(VS_INPUT vtx) {
	PS_INPUT ps;
	ps.pos = float4(vtx.pos, 1.0f);
	ps.uv = vtx.uv;
	ps.texid = vtx.texid;
	return ps;
}
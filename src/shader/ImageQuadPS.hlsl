#include "RootSignature.hlsl"

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float2 uv : TEXTCORDS;
	uint texid : TEXID;
};

struct PS_OUT {
	float4 color : SV_TARGET;
	float depth : SV_DEPTH;
};

// Texture and sampler
Texture2D<float4> textures[11] : register(t0);
sampler texSampler : register(s0);

[RootSignature(ROOTSIG)]
PS_OUT main(PS_INPUT vdin){
	PS_OUT psout;
	psout.color = textures[vdin.texid].Sample(texSampler, vdin.uv);
	psout.depth = psout.color.a > 0.5f ? vdin.pos.z : 0.0f;
	return psout;
}
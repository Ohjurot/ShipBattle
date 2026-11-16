// Binding contract shared by ALL shaders: every shader imports this module and declares
// no global resources of its own.
// The matching root signature is hardcoded in src/gfx/GShaderCompiler.cpp
// (WriteRootSignature) - change both together.
//
// Everything lives in the "Pipeline" namespace, so shaders access it as
// Pipeline::textures / Pipeline::texSampler and the origin is visible at the use site.
module Bindings;

namespace Pipeline {
	// Root parameter 0: descriptor table, t0..t10
	public Texture2D<float4> textures[11] : register(t0, space0);

	// Static sampler, s0
	public SamplerState texSampler : register(s0, space0);
}

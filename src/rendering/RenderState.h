#pragma once
#include "pch.h"

#include <gfx/GShader.h>

class RenderState {
	public:
		RenderState(ID3D12Device* ptrDevice, ID3D12RootSignature* ptrRootSignature, GShader& vs, GShader& ps);
		~RenderState();

		ID3D12PipelineState* getState();

	private:
		ScopedComPointer<ID3D12PipelineState> m_pso;
};
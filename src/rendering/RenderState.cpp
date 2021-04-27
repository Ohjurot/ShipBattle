#include "pch.h"
#include "RenderState.h"

RenderState::RenderState(ID3D12Device* ptrDevice, ID3D12RootSignature* ptrRootSignature, GShader& vs, GShader& ps) {
	// Input layout
	D3D12_INPUT_ELEMENT_DESC iaLayout[] = {
		{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"TEXTCORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"TEXID", 0, DXGI_FORMAT_R32_UINT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// Prepare PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC state;
	ZeroMemory(&state, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	// Desribe state
	state.pRootSignature = ptrRootSignature;
	state.VS.pShaderBytecode = vs.ptr();
	state.VS.BytecodeLength = vs.size();
	state.PS.pShaderBytecode = ps.ptr();
	state.PS.BytecodeLength = ps.size();
	state.StreamOutput.NumEntries = 0;
	state.StreamOutput.NumStrides = 0;
	state.BlendState.RenderTarget[0].BlendEnable = true;
	state.BlendState.RenderTarget[0].LogicOpEnable = false;
	state.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	state.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	state.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	state.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
	state.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	state.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	state.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	state.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	state.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	state.DepthStencilState.DepthEnable = true;
	state.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	state.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
	state.SampleMask = 0xFFFFFFFF;
	state.DepthStencilState.StencilEnable = false;
	state.InputLayout.NumElements = 3;
	state.InputLayout.pInputElementDescs = iaLayout;
	state.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	state.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	state.NumRenderTargets = 1;
	state.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	state.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	state.SampleDesc.Count = 1;
	state.SampleDesc.Quality = 0;
	state.NodeMask = NULL;
	state.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// Create pso
	ptrDevice->CreateGraphicsPipelineState(&state, IID_PPV_ARGS(m_pso.to()));
}

RenderState::~RenderState() {
	m_pso.release();
}

ID3D12PipelineState* RenderState::getState() {
	return m_pso;
}

#include "pch.h"
#include "GRootSignature.h"

GRootSignature::GRootSignature(ID3D12Device* ptrDevice, void* ptrData, UINT64 size) {
	ptrDevice->CreateRootSignature(NULL, ptrData, size, IID_PPV_ARGS(m_rootSignature.to()));
}

GRootSignature::~GRootSignature() {
	m_rootSignature.release();
}

ID3D12RootSignature* GRootSignature::getSignature() {
	return m_rootSignature;
}

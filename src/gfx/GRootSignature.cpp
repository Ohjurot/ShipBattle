#include "pch.h"
#include "GRootSignature.h"

GRootSignature::GRootSignature(ID3D12Device* ptrDevice, void* ptrData, UINT64 size) {
	HRESULT hr;
	if (FAILED(hr = ptrDevice->CreateRootSignature(NULL, ptrData, size, IID_PPV_ARGS(m_rootSignature.to())))) {
		GetLogger().log("ptrDevice->CreateRootSignature(...) failed!").log(hr);
		return;
	}
}

GRootSignature::~GRootSignature() {
	m_rootSignature.release();
}

ID3D12RootSignature* GRootSignature::getSignature() {
	return m_rootSignature;
}

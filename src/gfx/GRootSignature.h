#pragma once
#include "pch.h"

class GRootSignature {
	public:
		GRootSignature(ID3D12Device* ptrDevice, void* ptrData, UINT64 size);
		~GRootSignature();

		ID3D12RootSignature* getSignature();

	private:
		ScopedComPointer<ID3D12RootSignature> m_rootSignature;
};
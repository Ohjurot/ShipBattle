#pragma once
#include "pch.h"

class GHeapCpuAutoHandle {
	public:
		GHeapCpuAutoHandle(D3D12_CPU_DESCRIPTOR_HANDLE start, UINT64 increment);
		D3D12_CPU_DESCRIPTOR_HANDLE getIncrement();
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_handle;
		const UINT64 m_increment;
};

class GDescHeap {
	public:
		GDescHeap(ID3D12Device* ptrDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count);
		~GDescHeap();

		ID3D12DescriptorHeap* getHeap();

		GHeapCpuAutoHandle getCpuHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle();

	private:
		UINT64 m_handleIncrement;
		ScopedComPointer<ID3D12DescriptorHeap> m_heap;
};
#include "pch.h"
#include "GDescHeap.h"

GDescHeap::GDescHeap(ID3D12Device* ptrDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count) {
	D3D12_DESCRIPTOR_HEAP_DESC hd;
	ZeroMemory(&hd, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	hd.Type = type;
	hd.NumDescriptors = count;
	hd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hd.NodeMask = NULL;

	HRESULT hr;
	if (FAILED(hr = ptrDevice->CreateDescriptorHeap(&hd, IID_PPV_ARGS(m_heap.to())))) {
		GetLogger().log("ptrDevice->CreateDescriptorHeap(...) failed!").log(hr);
		return;
	}
	
	m_handleIncrement = ptrDevice->GetDescriptorHandleIncrementSize(type);
	if (m_handleIncrement == 0) {
		GetLogger().log("ptrDevice->GetDescriptorHandleIncrementSize(...) return 0!");
	}
}

GDescHeap::~GDescHeap() {
	m_heap.release();
}

ID3D12DescriptorHeap* GDescHeap::getHeap() {
	return m_heap;
}

GHeapCpuAutoHandle GDescHeap::getCpuHandle() {
	return GHeapCpuAutoHandle(m_heap->GetCPUDescriptorHandleForHeapStart(), m_handleIncrement);
}

D3D12_GPU_DESCRIPTOR_HANDLE GDescHeap::getGpuHandle() {
	return m_heap->GetGPUDescriptorHandleForHeapStart();
}

GHeapCpuAutoHandle::GHeapCpuAutoHandle(D3D12_CPU_DESCRIPTOR_HANDLE start, UINT64 increment) :
	m_handle(start),
	m_increment(increment)
{}

D3D12_CPU_DESCRIPTOR_HANDLE GHeapCpuAutoHandle::getIncrement() {
	auto handle = m_handle;
	m_handle.ptr += m_increment;
	return handle;
}

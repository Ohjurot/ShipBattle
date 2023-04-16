#include "pch.h"
#include "GHeap.h"

GHeap::GHeap(ID3D12Device* ptrDevice, D3D12_HEAP_TYPE type, UINT64 size) :
	m_size(size)
{
	// Desribe heap
	D3D12_HEAP_DESC hd;
	ZeroMemory(&hd, sizeof(D3D12_HEAP_DESC));
	hd.Properties.Type = type;
	hd.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hd.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hd.Properties.CreationNodeMask = NULL;
	hd.Properties.VisibleNodeMask = NULL;
	hd.SizeInBytes = size;
	hd.Alignment = 1024 * 64;
	hd.Flags = D3D12_HEAP_FLAG_NONE;

	// Create heap
	HRESULT hr;
	if (FAILED(hr = ptrDevice->CreateHeap(&hd, IID_PPV_ARGS(m_heap.to())))) {
		GetLogger().log("ptrDevice->CreateHeap(...) failed").log(hr);
		return;
	}
}

GHeap::~GHeap() {
	m_heap.release();
}

bool GHeap::heapAllocate(UINT64 size, UINT64* offset) {
	// Allocate on heap
	if (size > m_size - m_head) {
		return false;
	}

	// Set offset and increment 
	*offset = m_head;
	m_head += size;
	return true;
}

ID3D12Heap* GHeap::getHeap() {
	return m_heap;
}

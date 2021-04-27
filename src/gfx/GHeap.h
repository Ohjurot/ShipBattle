#pragma once
#include "pch.h"

class GHeap {
	public:
		GHeap(ID3D12Device* ptrDevice, D3D12_HEAP_TYPE type, UINT64 size);
		~GHeap();

		bool heapAllocate(UINT64 size, UINT64* offset);
		ID3D12Heap* getHeap();

	private:
		ScopedComPointer<ID3D12Heap> m_heap;
		const UINT64 m_size;
		UINT64 m_head = 0;
};
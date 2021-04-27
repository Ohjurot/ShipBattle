#pragma once
#include "pch.h"

class GContext {
	public:
		GContext();
		~GContext();

		void execute();
		void flush();

		ID3D12GraphicsCommandList* getList();
		ID3D12CommandQueue* getQueue();
		ID3D12Device* getDevice();

	private:
		ScopedComPointer<ID3D12Device> m_device;
		ScopedComPointer<ID3D12Fence> m_fence;

		UINT64 m_fenceValue = 0;
		ScopedComPointer<ID3D12CommandQueue> m_queue;
		ScopedComPointer<ID3D12CommandAllocator> m_allocator;
		ScopedComPointer<ID3D12GraphicsCommandList> m_cmdList;
};
#pragma once
#include "pch.h"

template<typename T, unsigned int N>
class GBuffer {
	public:
		GBuffer(ID3D12Device* ptrDevice, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_GENERIC_READ) {
			const UINT64 size = sizeof(T) * N;
			
			// Alloc cpu memory
			m_ptrCpu = (T*)malloc(size);
			
            // Desribe resource
            D3D12_RESOURCE_DESC rd;
            ZeroMemory(&rd, sizeof(D3D12_RESOURCE_DESC));
            rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            rd.Alignment = 0;
            rd.Width = size;
            rd.Height = 1;
            rd.DepthOrArraySize = 1;
            rd.MipLevels = 1;
            rd.Format = DXGI_FORMAT_UNKNOWN;
            rd.SampleDesc.Count = 1;
            rd.SampleDesc.Quality = 0;
            rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            rd.Flags = D3D12_RESOURCE_FLAG_NONE;

			// Heap
			D3D12_HEAP_PROPERTIES heapProperties;
			ZeroMemory(&heapProperties, sizeof(D3D12_HEAP_PROPERTIES));
			heapProperties.Type = heapType;
			heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProperties.CreationNodeMask = 0;
			heapProperties.VisibleNodeMask = 0;

            // Create resource
            HRESULT hr;
            if (FAILED(hr = ptrDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &rd, state, NULL, IID_PPV_ARGS(m_ptrGpu.to())))) {
                GetLogger().log("ptrDevice->CreatePlacedResource(...) failed!").log(hr);
                return;
            }
		}

		~GBuffer() {
			m_ptrGpu.release();
			free(m_ptrCpu);
		}

		void createVertexView(D3D12_VERTEX_BUFFER_VIEW* ptrViewDesc) {
			ptrViewDesc->BufferLocation = m_ptrGpu->GetGPUVirtualAddress();
			ptrViewDesc->SizeInBytes = size();
			ptrViewDesc->StrideInBytes = sizeof(T);
		}

		template<typename = std::enable_if_t<std::is_same_v<T, UINT32>>>
		void createIndexView(D3D12_INDEX_BUFFER_VIEW* ptrViewDesc) {
			ptrViewDesc->BufferLocation = m_ptrGpu->GetGPUVirtualAddress();
			ptrViewDesc->Format = DXGI_FORMAT_R32_UINT;
			ptrViewDesc->SizeInBytes = size();
		}

		void createConstView(D3D12_CONSTANT_BUFFER_VIEW_DESC* ptrViewDesc) {
			ptrViewDesc->BufferLocation = m_ptrGpu->GetGPUVirtualAddress();
			ptrViewDesc->SizeInBytes = size();
		}

		void uploadBufferCopy() {
			// Target
			D3D12_RANGE range;
			range.Begin = 0;
			range.End = size();
			void* ptrGpuBuffer;

			// Map copy unmap
			m_ptrGpu->Map(0, &range, &ptrGpuBuffer);
			memcpy(ptrGpuBuffer, m_ptrCpu, size());
			m_ptrGpu->Unmap(0, &range);
		}

		T& operator[](unsigned int idx) {
			return m_ptrCpu[idx];
		}

		T& get(unsigned int idx) {
			return m_ptrCpu[idx];
		}

		ID3D12Resource* getResource() {
			return m_ptrGpu;
		}

		void* getPtr() {
			return m_ptrCpu;
		}

		constexpr UINT64 size() {
			return sizeof(T) * N;
		}

	private:
		T* m_ptrCpu;
		ScopedComPointer<ID3D12Resource> m_ptrGpu;
};
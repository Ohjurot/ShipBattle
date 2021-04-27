#pragma once
#include "pch.h"

#include <tools/WICMeta.h>
#include <tools/WICImageProcessor.h>

#include <gfx/GHeap.h>
#include <gfx/GBuffer.h>
#include <gfx/GContext.h>

class GTexture {
	public:
		template<unsigned int N>
		GTexture(LPCWSTR path, GContext& refContext, GHeap& refHeap, GBuffer<BYTE, N>& refUploadBuffer) {
			// Open texture
			WIC_META fileMeta;
			if (!WicIO::open(path, fileMeta)) {
				return;
			}

			m_width = fileMeta.width;
			m_height = fileMeta.height;
			m_format = fileMeta.targetGiFormat;

			// Check size
			if (fileMeta.width * fileMeta.height * ((fileMeta.bpp + 7) / 8) > N) {
				return;
			}
			
			// Load texture
			if (!WICImageProcessor::wicToMemory(fileMeta, &refUploadBuffer[0], N)) {
				return;
			}

			// Create texturre
			D3D12_RESOURCE_DESC rd;
			ZeroMemory(&rd, sizeof(D3D12_RESOURCE_DESC));
			rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			rd.Width = fileMeta.width;
			rd.Height = fileMeta.height;
			rd.DepthOrArraySize = 1;
			rd.MipLevels = 1;
			rd.SampleDesc.Count = 1;
			rd.SampleDesc.Quality = 0;
			rd.Format = fileMeta.targetGiFormat;
			rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			rd.Flags = D3D12_RESOURCE_FLAG_NONE;
			rd.Alignment = 1024 * 64;

			// Check size and alloc
			UINT64 size = refContext.getDevice()->GetResourceAllocationInfo(NULL, 1, &rd).SizeInBytes;
			UINT64 offset;
			if (!refHeap.heapAllocate(size, &offset)) {
				return;
			}

			// Create texture
			if (FAILED(refContext.getDevice()->CreatePlacedResource(refHeap.getHeap(), offset, &rd, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(m_texture.to())))) {
				return;
			}
			m_texture.name(path);


			// Copy to GPU resource
			refUploadBuffer.uploadBufferCopy();

			// Location source
			D3D12_TEXTURE_COPY_LOCATION src;
			src.pResource = refUploadBuffer.getResource();
			src.PlacedFootprint.Offset = 0;
			src.PlacedFootprint.Footprint.Width = fileMeta.width;
			src.PlacedFootprint.Footprint.Height = fileMeta.height;
			src.PlacedFootprint.Footprint.Depth = 1;
			src.PlacedFootprint.Footprint.Format = fileMeta.targetGiFormat;
			src.PlacedFootprint.Footprint.RowPitch = fileMeta.stride;
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

			// Source box
			D3D12_BOX box;
			box.top = 0;
			box.left = 0;
			box.front = 0;
			box.bottom = fileMeta.height;
			box.right = fileMeta.width;
			box.back = 1;

			// Location dest
			D3D12_TEXTURE_COPY_LOCATION dest;
			dest.pResource = m_texture;
			dest.SubresourceIndex = 0;
			dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

			// Queue copy
			refContext.getList()->CopyTextureRegion(&dest, 0, 0, 0, &src, &box);
			
			// Barrier
			D3D12_RESOURCE_BARRIER barr;
			barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barr.Transition.pResource = m_texture;
			barr.Transition.Subresource = 0;
			barr.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barr.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
			refContext.getList()->ResourceBarrier(1, &barr);

			refContext.execute();
		}

		~GTexture();
		void createSrv(ID3D12Device* ptrDevice, D3D12_CPU_DESCRIPTOR_HANDLE handle);

	private:
		ScopedComPointer<ID3D12Resource> m_texture;
		UINT m_width, m_height;
		DXGI_FORMAT m_format;
};
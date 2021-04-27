#pragma once

#include <gfx/GContext.h>
#include <gfx/GHeap.h>
#include <gfx/GBuffer.h>

#include <rendering/Vertex.h>

class QuadManger {
	public:
		QuadManger(ID3D12Device* ptrDevice, GHeap& refHeapGpu, GHeap& refHeapUpload);

		void addQuad(TextureQuad& quad);
		UINT64 uploadReset(ID3D12GraphicsCommandList* ptrCmdList);

	private:
		GBuffer<TextureQueadVertex, 1024 * 6> m_vertexBuffer;
		GBuffer<TextureQueadVertex, 1024 * 6> m_uploadBuffer;
		UINT64 m_currentIndex = 0;
};

#include "pch.h"
#include "QuadManger.h"

QuadManger::QuadManger(ID3D12Device* ptrDevice) :
	m_vertexBuffer(ptrDevice, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON),
	m_uploadBuffer(ptrDevice, D3D12_HEAP_TYPE_UPLOAD)
{}

void QuadManger::addQuad(TextureQuad& quad) {
	float layer = quad.layer / 255.0f;
	
	// Requires 2 Triangle

	// T1 - V1 (Top Left)
	m_uploadBuffer[m_currentIndex + 0].pos[0] = quad.left;
	m_uploadBuffer[m_currentIndex + 0].pos[1] = quad.top;
	m_uploadBuffer[m_currentIndex + 0].pos[2] = layer;
	m_uploadBuffer[m_currentIndex + 0].uv[0] = quad.uv_left;
	m_uploadBuffer[m_currentIndex + 0].uv[1] = quad.uv_top;
	m_uploadBuffer[m_currentIndex + 0].texid = quad.texid;

	// T1 - V2 (Bottom Left)
	m_uploadBuffer[m_currentIndex + 1].pos[0] = quad.left;
	m_uploadBuffer[m_currentIndex + 1].pos[1] = quad.bottom;
	m_uploadBuffer[m_currentIndex + 1].pos[2] = layer;
	m_uploadBuffer[m_currentIndex + 1].uv[0] = quad.uv_left;
	m_uploadBuffer[m_currentIndex + 1].uv[1] = quad.uv_bottom;
	m_uploadBuffer[m_currentIndex + 1].texid = quad.texid;

	// T1 - V3 (Top Rigt)
	m_uploadBuffer[m_currentIndex + 2].pos[0] = quad.right;
	m_uploadBuffer[m_currentIndex + 2].pos[1] = quad.top;
	m_uploadBuffer[m_currentIndex + 2].pos[2] = layer;
	m_uploadBuffer[m_currentIndex + 2].uv[0] = quad.uv_right;
	m_uploadBuffer[m_currentIndex + 2].uv[1] = quad.uv_top;
	m_uploadBuffer[m_currentIndex + 2].texid = quad.texid;

	// T2 - V1 (Bottom Left)
	m_uploadBuffer[m_currentIndex + 3].pos[0] = quad.left;
	m_uploadBuffer[m_currentIndex + 3].pos[1] = quad.bottom;
	m_uploadBuffer[m_currentIndex + 3].pos[2] = layer;
	m_uploadBuffer[m_currentIndex + 3].uv[0] = quad.uv_left;
	m_uploadBuffer[m_currentIndex + 3].uv[1] = quad.uv_bottom;
	m_uploadBuffer[m_currentIndex + 3].texid = quad.texid;

	// T2 - V2 (Top Rigt)
	m_uploadBuffer[m_currentIndex + 4].pos[0] = quad.right;
	m_uploadBuffer[m_currentIndex + 4].pos[1] = quad.top;
	m_uploadBuffer[m_currentIndex + 4].pos[2] = layer;
	m_uploadBuffer[m_currentIndex + 4].uv[0] = quad.uv_right;
	m_uploadBuffer[m_currentIndex + 4].uv[1] = quad.uv_top;
	m_uploadBuffer[m_currentIndex + 4].texid = quad.texid;

	// T2 - V3 (Bottom Rigt)
	m_uploadBuffer[m_currentIndex + 5].pos[0] = quad.right;
	m_uploadBuffer[m_currentIndex + 5].pos[1] = quad.bottom;
	m_uploadBuffer[m_currentIndex + 5].pos[2] = layer;
	m_uploadBuffer[m_currentIndex + 5].uv[0] = quad.uv_right;
	m_uploadBuffer[m_currentIndex + 5].uv[1] = quad.uv_bottom;
	m_uploadBuffer[m_currentIndex + 5].texid = quad.texid;

	// Increment
	m_currentIndex += 6;
}

UINT64 QuadManger::uploadReset(ID3D12GraphicsCommandList* ptrCmdList) {
	if (!m_currentIndex) {
		return 0;
	}
	
	// Barrier
	D3D12_RESOURCE_BARRIER barr0;
	barr0.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barr0.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barr0.Transition.pResource = m_vertexBuffer.getResource();
	barr0.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barr0.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	barr0.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	ptrCmdList->ResourceBarrier(1, &barr0);

	// Upload
	m_uploadBuffer.uploadBufferCopy();
	ptrCmdList->CopyBufferRegion(m_vertexBuffer.getResource(), 0, m_uploadBuffer.getResource(), 0, sizeof(TextureQueadVertex) * m_currentIndex);

	// Barrier
	D3D12_RESOURCE_BARRIER barr1;
	barr1.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barr1.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barr1.Transition.pResource = m_vertexBuffer.getResource();
	barr1.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barr1.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barr1.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	ptrCmdList->ResourceBarrier(1, &barr1);

	// Bind vb view
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = m_vertexBuffer.getResource()->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(TextureQueadVertex) * m_currentIndex;
	vbv.StrideInBytes = sizeof(TextureQueadVertex);
	ptrCmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ptrCmdList->IASetVertexBuffers(0, 1, &vbv);

	// Reset
	auto lastCount = m_currentIndex;
	m_currentIndex = 0;
	return lastCount;
}

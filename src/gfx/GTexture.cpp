#include "pch.h"
#include "GTexture.h"

GTexture::~GTexture() {
	m_texture.release();
}

void GTexture::createSrv(ID3D12Device* ptrDevice, D3D12_CPU_DESCRIPTOR_HANDLE handle) {
	D3D12_SHADER_RESOURCE_VIEW_DESC vd;
	ZeroMemory(&vd, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
	vd.Format = m_format;
	vd.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	vd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	vd.Texture2D.MipLevels = 1;
	vd.Texture2D.MostDetailedMip = 0;
	vd.Texture2D.PlaneSlice = 0;
	vd.Texture2D.ResourceMinLODClamp = 0.0f;

	ptrDevice->CreateShaderResourceView(m_texture, &vd, handle);
}

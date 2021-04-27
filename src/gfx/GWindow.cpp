#include "pch.h"
#include "GWindow.h"

GWindow::GWindow(EasyHWND::WindowClass& wClass, ID3D12Device* ptrDevice, ID3D12CommandQueue* ptrQueue)
	: EasyHWND::Window(wClass, L"Ship Battle V1.0 © Copyright 2021 by Ludwig Füchsl", 100, 100, 1920, 1080, WS_CAPTION | WS_SYSMENU),
	m_iostate({})
{
	// Adjust windows size
	UINT width = 1920;
	UINT height = 1080;
	adjustSizeForWindow(&width, &height);

	// Set size
	setWindowSize(width, height);

	// Create GI Factory
	ScopedComPointer<IDXGIFactory2> ptrFactory;
	if (FAILED(CreateDXGIFactory2(NULL, IID_PPV_ARGS(ptrFactory.to())))) {
		return;
	}

	// Describe swap chain
	DXGI_SWAP_CHAIN_DESC1 ds;
	ZeroMemory(&ds, sizeof(DXGI_SWAP_CHAIN_DESC1));
	ds.Width = 1920;
	ds.Height = 1080;
	ds.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ds.Stereo = false;
	ds.SampleDesc.Count = 1;
	ds.SampleDesc.Quality = 0;
	ds.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	ds.BufferCount = 2;
	ds.Scaling = DXGI_SCALING_NONE;
	ds.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	ds.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	ds.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	// Fullscreen
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd;
	ZeroMemory(&fd, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	fd.Windowed = true;

	// Create Swap Chain
	if (FAILED(ptrFactory->CreateSwapChainForHwnd(ptrQueue, this->operator HWND(), &ds, &fd, NULL, m_swapChain.to()))) {
		return;
	}

	// Create DESC heap
	D3D12_DESCRIPTOR_HEAP_DESC hd;
	ZeroMemory(&hd, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	hd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	hd.NumDescriptors = 2;
	hd.NodeMask = NULL;
	hd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(ptrDevice->CreateDescriptorHeap(&hd, IID_PPV_ARGS(m_descHeap.to())))) {
		return;
	}

	// Heap increment
	m_heapIncrement = ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Get buffers
	for (unsigned int i = 0; i < 2; i++) {
		// Get buffer
		if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_buffers[i].to())))) {
			return;
		}

		// Create view
		D3D12_CPU_DESCRIPTOR_HANDLE heapHandle = m_descHeap->GetCPUDescriptorHandleForHeapStart();
		heapHandle.ptr += i * m_heapIncrement;
		ptrDevice->CreateRenderTargetView(m_buffers[i], NULL, heapHandle);
	}

	// Depth HEAP
	D3D12_DESCRIPTOR_HEAP_DESC hdd;
	ZeroMemory(&hdd, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	hdd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hdd.NumDescriptors = 1;
	hdd.NodeMask = NULL;
	hdd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(ptrDevice->CreateDescriptorHeap(&hdd, IID_PPV_ARGS(m_depthHeap.to())))) {
		return;
	}

	// DB Heap
	D3D12_HEAP_PROPERTIES hprop;
	hprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hprop.Type = D3D12_HEAP_TYPE_DEFAULT;
	hprop.CreationNodeMask = NULL;
	hprop.VisibleNodeMask = NULL;

	// Depth buffer
	D3D12_RESOURCE_DESC dbd;
	ZeroMemory(&dbd, sizeof(D3D12_RESOURCE_DESC));
	dbd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dbd.Alignment = 1024 * 64;
	dbd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dbd.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	dbd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	dbd.MipLevels = 1;
	dbd.SampleDesc.Count = 1;
	dbd.SampleDesc.Quality = 0;
	dbd.Height = 1080;
	dbd.Width = 1920;
	dbd.DepthOrArraySize = 1;

	// Clear value
	D3D12_CLEAR_VALUE dbdc;
	dbdc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dbdc.DepthStencil.Depth = 0.0f;

	ptrDevice->CreateCommittedResource(&hprop, D3D12_HEAP_FLAG_NONE, &dbd, D3D12_RESOURCE_STATE_DEPTH_WRITE, &dbdc, IID_PPV_ARGS(m_depthBuffer.to()));

	// Create view
	ptrDevice->CreateDepthStencilView(m_depthBuffer, NULL, m_depthHeap->GetCPUDescriptorHandleForHeapStart());
}

GWindow::~GWindow(){
	m_descHeap.release();
	m_buffers[0].release();
	m_buffers[1].release();
	m_swapChain.release();

}

bool GWindow::handleWindowMessage(LRESULT* ptrLRESULT, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// Switch on message
	switch (msg) {
		// Key down
		case WM_KEYDOWN: {
			// Check enter
			if (wParam == VK_RETURN) {
				m_iostate.enterIsDown = TRUE;
			}
			break;
		}

		// Key up
		case WM_KEYUP: {
			// Check enter
			if (wParam == VK_RETURN) {
				m_iostate.enterIsDown = FALSE;
			}
			break;
		}

		// Mouse Button down
		case WM_LBUTTONDOWN: {
			m_iostate.mouseIsDown = TRUE;
			break;
		}

		// Mouse Button down
		case WM_LBUTTONUP: {
			m_iostate.mouseIsDown = FALSE;
			break;
		}

		// NC Area
		case WM_NCMOUSEMOVE: {
			ShowCursor(TRUE);
			break;
		}

		// Mouse move
		case WM_MOUSEMOVE: {
			// Get position
			ShowCursor(FALSE);
			m_iostate.mouseX = LOWORD(lParam);
			m_iostate.mouseY = HIWORD(lParam);
			break;
		}

		// Mouse exit
		case WM_MOUSELEAVE: {
			ShowCursor(TRUE);
			break;
		}
	}

	// Default (parent) handler
	return EasyHWND::Window::handleWindowMessage(ptrLRESULT, hwnd, msg, wParam, lParam);
}

void GWindow::beginFrame(ID3D12GraphicsCommandList* ptrCmdList) {	
	// Resource barrier
	D3D12_RESOURCE_BARRIER barr;
	ZeroMemory(&barr, sizeof(D3D12_RESOURCE_BARRIER));
	barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barr.Transition.pResource = m_buffers[m_currentBuffer];
	barr.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barr.Transition.Subresource = 0;
	barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// Queue barrier
	ptrCmdList->ResourceBarrier(1, &barr);

	// RTV Handle
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_descHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_heapIncrement * m_currentBuffer;

	// Clear RTV
	const float rtvCC[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ptrCmdList->ClearRenderTargetView(rtvHandle, rtvCC, 0, NULL);
	
	// Clear DSV
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_depthHeap->GetCPUDescriptorHandleForHeapStart();
	ptrCmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 0.0f, NULL, 0, NULL);

	// Set RTV
	ptrCmdList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
}

void GWindow::endFrame(ID3D12GraphicsCommandList* ptrCmdList) {
	// Resource barrier
	D3D12_RESOURCE_BARRIER barr;
	ZeroMemory(&barr, sizeof(D3D12_RESOURCE_BARRIER));
	barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barr.Transition.pResource = m_buffers[m_currentBuffer];
	barr.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barr.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barr.Transition.Subresource = 0;
	barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// Queue barrier
	ptrCmdList->ResourceBarrier(1, &barr);
}

bool GWindow::present() {
	m_currentBuffer = (m_currentBuffer + 1) % 2;
	return SUCCEEDED(m_swapChain->Present(1, 0));
}

GameIOState GWindow::getCurrentIOState() {
	return m_iostate;
}

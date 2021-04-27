#pragma once
#include "pch.h"

#include <game/IOState.h>

class GWindow : public EasyHWND::Window{
	public:
		GWindow(EasyHWND::WindowClass& wClass, ID3D12Device* ptrDevice, ID3D12CommandQueue* ptrQueue);
		~GWindow();

		bool handleWindowMessage(LRESULT* ptrLRESULT, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		void beginFrame(ID3D12GraphicsCommandList* ptrCmdList);
		void endFrame(ID3D12GraphicsCommandList* ptrCmdList);
		bool present();

		GameIOState getCurrentIOState();

	private:
		GameIOState m_iostate;

		unsigned int m_currentBuffer = 0;
		unsigned int m_heapIncrement = 0;
		ScopedComPointer<IDXGISwapChain1> m_swapChain;
		ScopedComPointer<ID3D12DescriptorHeap> m_descHeap;
		ScopedComPointer<ID3D12Resource> m_buffers[2];

		ScopedComPointer<ID3D12Resource> m_depthBuffer;
		ScopedComPointer<ID3D12DescriptorHeap> m_depthHeap;
};
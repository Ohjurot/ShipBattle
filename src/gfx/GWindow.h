#pragma once
#include "pch.h"

#include <game/IOState.h>

class GWindow : public EasyHWND::Window{
	public:
		GWindow(EasyHWND::WindowClass& wClass, ID3D12Device* ptrDevice, ID3D12CommandQueue* ptrQueue);
		~GWindow();

		bool handleWindowMessage(LRESULT* ptrLRESULT, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		void resize(ID3D12Device* device);

		void beginFrame(ID3D12GraphicsCommandList* ptrCmdList);
		void endFrame(ID3D12GraphicsCommandList* ptrCmdList);
		bool present();

		GameIOState getCurrentIOState();

		inline bool needsResizing() const { return m_needsResize;  }
		inline int getWidth() const { return m_width;  }
		inline int getHeight() const { return m_height;  }

	private:
		void createDepthBuffer(ID3D12Device* device);
		void destroyDepthBuffer();

		void getBuffers(ID3D12Device* device);
		void releaseBuffers();

	private:
		GameIOState m_iostate;

		bool m_needsResize = false;

		unsigned int m_heapIncrement = 0;

		int m_width = -1;
		int m_height = -1;

		ScopedComPointer<IDXGISwapChain4> m_swapChain;
		ScopedComPointer<ID3D12DescriptorHeap> m_descHeap;
		ScopedComPointer<ID3D12Resource> m_buffers[2];

		ScopedComPointer<ID3D12Resource> m_depthBuffer;
		ScopedComPointer<ID3D12DescriptorHeap> m_depthHeap;
};
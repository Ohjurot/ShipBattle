#include "pch.h"
#include "GContext.h"

GContext::GContext() {
    HRESULT hr;

    // Get factory
    ScopedComPointer<IDXGIFactory7> factory;
    if (FAILED(hr = CreateDXGIFactory2(0, IID_PPV_ARGS(factory.to())))) {
        GetLogger().log("CreateDXGIFactory2(...) failed!").log(hr);
        return;
    }

    // Get most performant adapter
    ScopedComPointer<IDXGIAdapter4> adapter;
    if (FAILED(hr = factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.to())))) {
        GetLogger().log("factory->EnumAdapterByGpuPreference(...) failed!").log(hr);
        return;
    }

    // Create device
    if (FAILED(hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_device.to())))) {
        GetLogger().log("D3D12CreateDevice(...) failed!").log(hr).log("Using WARP adapter!");
        
        DXGI_ADAPTER_DESC3 ad{};
        adapter->GetDesc3(&ad);

        // Try warp adapter
        if (FAILED(hr = factory->EnumWarpAdapter(IID_PPV_ARGS(adapter.to()))))
        {
            GetLogger().log("factory->EnumWarpAdapter(...) failed!").log(hr);
            return;
        }

        // Final try
        if (FAILED(hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_device.to()))))
        {
            GetLogger().log("D3D12CreateDevice(...) failed!").log(hr);
            return;
        }

        // Complain to the user
        std::wstringstream wss;
        wss << "The game failed to activate your GPU \"" << ad.Description << "\"" << std::endl
            << "Please make sure you have a gpu with DirectX 12 support (And feature level 11.0) installed!" << std::endl
            << "Using the Microsoft Software renderer! Bad performance is expected!";
        MessageBoxW(nullptr, wss.str().c_str(), L"D3D12CreateDevice(...) failed!", MB_OK | MB_ICONERROR);
    }

    // Create fence
    if (FAILED(hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.to())))) {
        GetLogger().log("m_device->CreateFence(...) failed!").log(hr);
        return;
    }

    // Desribe queue
    D3D12_COMMAND_QUEUE_DESC qd;
    ZeroMemory(&qd, sizeof(D3D12_COMMAND_QUEUE_DESC));
    qd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    qd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    qd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    qd.NodeMask = NULL;
    
    // Create queue
    if (FAILED(hr = m_device->CreateCommandQueue(&qd, IID_PPV_ARGS(m_queue.to())))) {
        GetLogger().log("m_device->CreateCommandQueue(...) failed!").log(hr);
        return;
    }

    // create allocator
    if (FAILED(hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_allocator.to())))) {
        GetLogger().log("m_device->CreateCommandAllocator(...) failed!").log(hr);
        return;
    }

    // Create list
    if (FAILED(hr = m_device->CreateCommandList(NULL, D3D12_COMMAND_LIST_TYPE_DIRECT, m_allocator, NULL, IID_PPV_ARGS(m_cmdList.to())))) {
        GetLogger().log("m_device->CreateCommandList(...) failed!").log(hr);
        return;
    }
}

GContext::~GContext() {
    // Make sure to execute
    execute();
    flush();

    // Release
    m_cmdList.release();
    m_allocator.release();
    m_queue.release();
    m_fence.release();
    m_device.release();
}

void GContext::execute() {
    // Close
    m_cmdList->Close();
      
    // Execute
    ID3D12CommandList* lists[] = {
        m_cmdList
    };
    m_queue->ExecuteCommandLists(1, lists);
    m_queue->Signal(m_fence, ++m_fenceValue);

    // Wait
    HANDLE e = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (e && SUCCEEDED(m_fence->SetEventOnCompletion(m_fenceValue, e))) {
        WaitForSingleObject(e, INFINITE);
    }
    else {
        
    }

    // Reset
    m_allocator->Reset();
    m_cmdList->Reset(m_allocator, NULL);
}

void GContext::flush() {
    // Final release
    for (unsigned int i = 0; i < 2; i++) {
        m_queue->Signal(m_fence, ++m_fenceValue);
        while (m_fence->GetCompletedValue() < m_fenceValue) SwitchToThread();
    }
}

ID3D12GraphicsCommandList* GContext::getList() {
    return m_cmdList;
}

ID3D12CommandQueue* GContext::getQueue() {
    return m_queue;
}

ID3D12Device* GContext::getDevice() {
    return m_device;
}

#include "pch.h"
#include "GContext.h"

GContext::GContext() {
    // Create device
    if (FAILED(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_device.to())))) {
        return;
    }

    // Create fence
    m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.to()));

    // Desribe queue
    D3D12_COMMAND_QUEUE_DESC qd;
    ZeroMemory(&qd, sizeof(D3D12_COMMAND_QUEUE_DESC));
    qd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    qd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    qd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    qd.NodeMask = NULL;
    
    // Create queue
    if (FAILED(m_device->CreateCommandQueue(&qd, IID_PPV_ARGS(m_queue.to())))) {
            return;
    }

    // create allocator
    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_allocator.to())))) {
        return;
    }

    // Create list
    if (FAILED(m_device->CreateCommandList(NULL, D3D12_COMMAND_LIST_TYPE_DIRECT, m_allocator, NULL, IID_PPV_ARGS(m_cmdList.to())))) {
        return;
    }
}

GContext::~GContext() {
    // Make shure to execute
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

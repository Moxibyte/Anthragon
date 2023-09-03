#include "d3d_descriptor_heap.h"

ant::sdf::d3d_descriptor_heap::d3d_descriptor_heap(d3d_context::ptr ctx, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count, bool shader_visible) :
    m_type(type), m_count(count)
{
    m_increment = ctx->get_device()->GetDescriptorHandleIncrementSize(type);

    D3D12_DESCRIPTOR_HEAP_DESC hd;
    hd.Type = type;
    hd.NumDescriptors = count;
    hd.Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    hd.NodeMask = 0;
    ANT_CHECK_HR(ctx->get_device()->CreateDescriptorHeap(&hd, IID_PPV_ARGS(&m_heap)), "Failed to create descriptor heap");

    m_handle_cpu = m_heap->GetCPUDescriptorHandleForHeapStart();
    m_handle_gpu = m_heap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE ant::sdf::d3d_descriptor_heap::get_cpu(size_t index)
{
    ANT_CHECK(index < m_count, "Index out of range");

    auto handle = m_handle_cpu;
    handle.ptr += index * m_increment;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE ant::sdf::d3d_descriptor_heap::get_gpu(size_t index)
{
    ANT_CHECK(index < m_count, "Index out of range");

    auto handle = m_handle_gpu;
    handle.ptr += index * m_increment;
    return handle;
}

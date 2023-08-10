#include "d3d_resource.h"

ant::sdf::d3d_resource::d3d_resource(d3d_context::ptr ctx, size_t size, D3D12_HEAP_TYPE heap, D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state) :
    m_ctx(ctx), m_heap(heap), m_size(size), m_state(state)
{
    D3D12_HEAP_PROPERTIES hp;
    hp.Type = heap;
    hp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    hp.CreationNodeMask = hp.VisibleNodeMask = 0;

    ANT_CHECK_HR(
        m_ctx->get_device()->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &desc, state, nullptr, IID_PPV_ARGS(&m_resource)), 
        "Buffer creation failed!"
    );
}

void ant::sdf::d3d_resource::state_transition(d3d_command_list::ptr list, D3D12_RESOURCE_STATES state)
{
    if (m_state != state)
    {
        D3D12_RESOURCE_BARRIER barr;
        barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barr.Transition.pResource = m_resource;
        barr.Transition.Subresource = 0;
        barr.Transition.StateBefore = m_state;
        barr.Transition.StateAfter = state;
        barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

        list->add_resource_barrier(barr);
        m_state = state;
    }
}

void* ant::sdf::d3d_resource::map()
{
    void* map = nullptr;
    ANT_CHECK_HR(m_resource->Map(0, nullptr, &map), "Failed to map resource");
    return map;
}

void ant::sdf::d3d_resource::unmap()
{
    m_resource->Unmap(0, nullptr);
}

ant::sdf::d3d_resource::ptr ant::sdf::d3d_resource::create_upload_buffer(d3d_context::ptr ctx, size_t size)
{
    return create_buffer(ctx, size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
}

ant::sdf::d3d_resource::ptr ant::sdf::d3d_resource::create_default_buffer(d3d_context::ptr ctx, size_t size)
{
    return create_buffer(ctx, size, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
}

ant::sdf::d3d_resource::ptr ant::sdf::d3d_resource::create_buffer(d3d_context::ptr ctx, size_t size, D3D12_HEAP_TYPE heap, D3D12_RESOURCE_STATES state)
{
    size_t real_size = pad_size(size, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

    D3D12_RESOURCE_DESC rd;
    rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    rd.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    rd.Width = real_size;
    rd.Height = 1;
    rd.DepthOrArraySize = 1;
    rd.MipLevels = 1;
    rd.Format = DXGI_FORMAT_UNKNOWN;
    rd.SampleDesc.Count = 1;
    rd.SampleDesc.Quality = 0;
    rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    rd.Flags = D3D12_RESOURCE_FLAG_NONE;

    return std::make_shared<d3d_resource>(ctx, real_size, heap, std::reference_wrapper(rd), state);
}

size_t ant::sdf::d3d_resource::pad_size(size_t in, size_t pad)
{
    size_t d = in % pad;
    return d == 0 ? in : in + pad - d;
}


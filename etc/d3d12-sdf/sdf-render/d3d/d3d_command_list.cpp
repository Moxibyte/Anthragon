#include "d3d_command_list.h"

ant::sdf::d3d_command_list::d3d_command_list(d3d_context::ptr ctx) :
    m_ctx(ctx)
{
    ANT_CHECK_HR(m_ctx->get_device()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_allocator)), "Failed to create command allocator");
    ANT_CHECK_HR(m_ctx->get_device()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_allocator, nullptr, IID_PPV_ARGS(&m_cmd_list)), "Failed to create command list");
}

ant::sdf::d3d_command_list::~d3d_command_list()
{
    if (is_executing())
    {
        execute_async_wait();
    }
    m_cmd_list.Release();
    m_allocator.Release();
}

void ant::sdf::d3d_command_list::add_resource_barrier(D3D12_RESOURCE_BARRIER& barr)
{
    if (m_valid_barriers == m_barriers.size())
    {
        m_barriers.resize(std::max<size_t>(8, 2 * m_barriers.size()));
    }
    auto& barrier = m_barriers[m_valid_barriers++];
    memcpy(&barrier, &barr, sizeof(D3D12_RESOURCE_BARRIER));
}

void ant::sdf::d3d_command_list::stage_resource_barriers()
{
    if (m_valid_barriers)
    {
        m_cmd_list->ResourceBarrier(m_valid_barriers, m_barriers.data());
        m_valid_barriers = 0;
    }
}

void ant::sdf::d3d_command_list::clear_rtv(D3D12_CPU_DESCRIPTOR_HANDLE rtv)
{
    float color[] = { .0f, .0f, .0f, .0f, };
    m_cmd_list->ClearRenderTargetView(rtv, color, 0, nullptr);
}

void ant::sdf::d3d_command_list::ia_set_vertex_buffer(size_t size, size_t stride, D3D12_GPU_VIRTUAL_ADDRESS address)
{
    D3D12_VERTEX_BUFFER_VIEW vbv{};
    vbv.BufferLocation = address;
    vbv.SizeInBytes = size;
    vbv.StrideInBytes = stride;
    
    m_cmd_list->IASetVertexBuffers(0, 1, &vbv);
}

void ant::sdf::d3d_command_list::execute_sync(d3d_executor::ptr executor)
{
    ANT_CHECK(is_executing() == false, "Can't dispatch executing while already executing");

    m_active_executor = executor;
    dispatch_executor();
    m_active_executor->wait_for_cookie(m_completion_cookie);
    m_active_executor = nullptr;
}

void ant::sdf::d3d_command_list::execute_async(d3d_executor::ptr executor)
{
    ANT_CHECK(is_executing() == false, "Can't dispatch executing while already executing");

    m_active_executor = executor;
    dispatch_executor();
}

void ant::sdf::d3d_command_list::execute_async_wait()
{
    if (m_active_executor)
    {
        m_active_executor->wait_for_cookie(m_completion_cookie);
        m_active_executor = nullptr;
    }
}

void ant::sdf::d3d_command_list::reset()
{
    ANT_CHECK(is_executing() == false, "Can't reset while executing");
    ANT_CHECK_HR(m_allocator->Reset(), "Can't reset allocator");
    ANT_CHECK_HR(m_cmd_list->Reset(m_allocator, nullptr), "Can't reset command list");
}

bool ant::sdf::d3d_command_list::is_executing()
{
    if (m_active_executor)
    {
        if (m_active_executor->check_cookie_completion(m_completion_cookie))
        {
            m_active_executor = nullptr;
        }
        else
        {
            return true;
        }
    }
    return false;
}

void ant::sdf::d3d_command_list::dispatch_executor()
{
    // Prepare for execution
    stage_resource_barriers();

    ANT_CHECK_HR(m_cmd_list->Close(), "Can't close command list");
    m_completion_cookie = m_active_executor->execute(m_cmd_list);
}

void ant::sdf::d3d_command_list::copy_texture(ID3D12Resource* src, ID3D12Resource* dst, uint32_t width, uint32_t height, uint32_t bpp, DXGI_FORMAT format)
{
    D3D12_TEXTURE_COPY_LOCATION src_location;
    src_location.pResource = src;
    src_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src_location.PlacedFootprint.Offset = 0;
    src_location.PlacedFootprint.Footprint.Width = width;
    src_location.PlacedFootprint.Footprint.Height = height;
    src_location.PlacedFootprint.Footprint.Depth = 1;
    src_location.PlacedFootprint.Footprint.Format = format;
    src_location.PlacedFootprint.Footprint.RowPitch = width * ((bpp + 7) / 8);

    D3D12_TEXTURE_COPY_LOCATION dst_location;
    dst_location.pResource = dst;
    dst_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst_location.SubresourceIndex = 0;

    D3D12_BOX box;
    box.left = box.top = box.front = 0;
    box.right = width;
    box.bottom = height;
    box.back = 1;

    m_cmd_list->CopyTextureRegion(&dst_location, 0, 0, 0, &src_location, &box);
}

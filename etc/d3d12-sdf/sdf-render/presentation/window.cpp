#include "window.h"

ant::sdf::window::window(d3d_context::ptr ctx, d3d_executor::ptr queue, std::wstring_view name, DWORD style, DWORD ex_style, int x, int y, int width, int height) :
    m_ctx(ctx)
{
    m_wnd = window_factory::get().new_window(
        name, style, ex_style, x, y, width, height,
        std::bind(&window::msg_proc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
    );
    ANT_CHECK(m_wnd != nullptr, "Failed to create window");

    DXGI_SWAP_CHAIN_DESC1 swd{};
    swd.Width = width;
    swd.Height = height;
    swd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swd.Stereo = false;
    swd.SampleDesc.Count = 1;
    swd.SampleDesc.Quality = 0;
    swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swd.BufferCount = 2;
    swd.Scaling = DXGI_SCALING_STRETCH;
    swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    swd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsd{};
    fsd.RefreshRate.Numerator = 1;
    fsd.RefreshRate.Denominator = 60;
    fsd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    fsd.Scaling = DXGI_MODE_SCALING_STRETCHED;
    fsd.Windowed = true;

    comptr<IDXGISwapChain1> swap_chain_1;
    ANT_CHECK_HR(m_ctx->get_factory()->CreateSwapChainForHwnd(queue->get_queue_ptr(), m_wnd, &swd, &fsd, nullptr, &swap_chain_1), "Failed to create swap chain");
    ANT_CHECK_HR(swap_chain_1->QueryInterface(&m_swap_chain), "Swap chain is not capable enought");

    m_rtv_heap = std::make_shared<d3d_descriptor_heap>(m_ctx, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
    get_buffers();
}

ant::sdf::window::~window()
{
    m_swap_chain.Release();

    if (m_wnd)
    {
        DestroyWindow(m_wnd);
    }
}

void ant::sdf::window::begin_frame(d3d_command_list::ptr cmd_list)
{
    D3D12_RESOURCE_BARRIER barr;
    barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barr.Transition.pResource = m_buffers[m_buffer_index];
    barr.Transition.Subresource = 0;
    barr.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    cmd_list->add_resource_barrier(barr);
    cmd_list->stage_resource_barriers();

    auto rtv_handle = m_rtv_heap->get_cpu(m_buffer_index);
    cmd_list->clear_rtv(rtv_handle);
    cmd_list->om_set_rtv(rtv_handle);
}

void ant::sdf::window::end_frame(d3d_command_list::ptr cmd_list)
{
    D3D12_RESOURCE_BARRIER barr;
    barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barr.Transition.pResource = m_buffers[m_buffer_index];
    barr.Transition.Subresource = 0;
    barr.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barr.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    cmd_list->add_resource_barrier(barr);
    cmd_list->stage_resource_barriers();
}

void ant::sdf::window::resize()
{
    release_buffers();

    ANT_CHECK_HR(m_swap_chain->ResizeBuffers(2, m_width, m_height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH), "Failed to resize swap chain");
    m_buffer_index = m_swap_chain->GetCurrentBackBufferIndex();

    get_buffers();
}

void ant::sdf::window::present()
{
    ANT_CHECK_HR(m_swap_chain->Present(1, 0), "Present failed");
    m_buffer_index = m_swap_chain->GetCurrentBackBufferIndex();
}

void ant::sdf::window::process_messages()
{
    MSG msg;
    while (PeekMessageW(&msg, m_wnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void ant::sdf::window::get_buffers()
{
    for (UINT i = 0; i < 2; i++)
    {
        ANT_CHECK_HR(m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i])), "Failed to retrive buffer");
        m_ctx->get_device()->CreateRenderTargetView(m_buffers[i], nullptr, m_rtv_heap->get_cpu(i));
    }
}

void ant::sdf::window::release_buffers()
{
    m_buffers[0].Release();
    m_buffers[1].Release();
}

LRESULT ant::sdf::window::msg_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
        case WM_CLOSE:
            m_close_received = true;
            return 0;

        case WM_SIZE:
            if (w_param == SIZE_RESTORED || w_param == SIZE_MAXIMIZED)
            {
                auto width = LOWORD(l_param);
                auto height = HIWORD(l_param);
                if (width != m_width || height != m_height)
                {
                    if (width >= 32 && height >= 32)
                    {
                        m_width = width;
                        m_height = height;
                        m_resized = true;
                    }
                }
            }
            break;
    }
    return DefWindowProcW(wnd, msg, w_param, l_param);
}

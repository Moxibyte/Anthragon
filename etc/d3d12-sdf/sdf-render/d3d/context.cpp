#include "context.h"

ant::sdf::d3d_context::d3d_context()
{
    #ifdef _DEBUG
    ANT_CHECK_HR(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d_debug)), "Failed to retrive d3d12 debug interface");
    m_d3d_debug->EnableDebugLayer();
    // m_d3d_debug->SetEnableAutoName(true);
    
    ANT_CHECK_HR(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgi_debug)), "Failed to retrive dxgi debug interface");
    m_dxgi_debug->EnableLeakTrackingForThread();
    #endif

    ANT_CHECK_HR(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgi_factory)), "Failed to create dxgi factory");

    comptr<IDXGIAdapter4> adapter;
    ANT_CHECK_HR(m_dxgi_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)), "Failed to retrive gpu");

    ANT_CHECK_HR(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)), "Failed to create gpu instance!");
}

ant::sdf::d3d_context::~d3d_context()
{
    m_device.Release();

    #ifdef _DEBUG
    if (m_dxgi_debug)
    {
        ANT_CHECK_HR(m_dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL)), "dxgi rldo failed!");
        m_dxgi_debug->DisableLeakTrackingForThread();
    }
    m_dxgi_debug.Release();

    m_d3d_debug.Release();
    #endif
}

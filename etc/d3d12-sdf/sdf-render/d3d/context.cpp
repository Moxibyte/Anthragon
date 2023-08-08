#include "context.h"

ant::sdf::d3d_context::d3d_context()
{
    #ifdef _DEBUG
    ANT_CHECK_HR(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d_debug)), "Failed to retrive d3d12 debug interface");
    m_d3d_debug->EnableDebugLayer();
    // m_d3d_debug->SetEnableAutoName(true);
    
    ANT_CHECK_HR(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgi_debug)), "Failed to retrive dxgi debug interface");
    m_dxgi_debug->EnableLeakTrackingForThread();
    ANT_CHECK_HR(m_dxgi_debug->QueryInterface(IID_PPV_ARGS(&m_dxgi_info_queue)), "Failed to retrive info queue");
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
        m_dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL));
        m_dxgi_debug->DisableLeakTrackingForThread();
    }
    m_dxgi_debug.Release();

    m_d3d_debug.Release();
    #endif
}

void ant::sdf::d3d_context::pull_messages()
{
    #ifdef _DEBUG
    std::vector<char> buffer;

    auto count = m_dxgi_info_queue->GetNumStoredMessages(DXGI_DEBUG_ALL);
    for (size_t i = 0; i < count; i++)
    {
        size_t msg_size = 0;
        m_dxgi_info_queue->GetMessageW(DXGI_DEBUG_ALL, i, nullptr, &msg_size);
        if (buffer.size() < msg_size) buffer.resize(msg_size);
        
        auto* msg = (DXGI_INFO_QUEUE_MESSAGE*)buffer.data();
        ANT_CHECK_HR(m_dxgi_info_queue->GetMessageW(DXGI_DEBUG_ALL, i, msg, &msg_size), "Failed to retrive debug message");
        
        std::wstring severity;
        switch (msg->Severity)
        {
            case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE:
                severity = L"MESSAGE";
                break;
            case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
                severity = L"INFO";
                break;
            case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
                severity = L"WARNING";
                break;
            case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
                severity = L"ERROR";
                break;
            case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
                severity = L"CORRUPTION";
                break;

        }

        std::wstring source;
        if(memcmp(&msg->Producer, &DXGI_DEBUG_D3D12, sizeof(GUID)) == 0)
        {
            source = L"D3D12";
        }
        else if (memcmp(&msg->Producer, &DXGI_DEBUG_APP, sizeof(GUID)) == 0)
        {
            source = L"APP";
        }
        else if (memcmp(&msg->Producer, &DXGI_DEBUG_DXGI, sizeof(GUID)) == 0)
        {
            source = L"DXGI";
        }

        std::wcout << L"[" << severity << L"] [" << source << L"] " << msg->pDescription << std::endl;
    }

    m_dxgi_info_queue->ClearStoredMessages(DXGI_DEBUG_ALL);
    #endif
}

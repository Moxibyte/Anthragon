#include "d3d_executor.h"

ant::sdf::d3d_executor::d3d_executor(d3d_context::ptr ctx) :
    m_ctx(ctx)
{
    m_event = CreateEventW(nullptr, false, false, nullptr);
    ANT_CHECK_LAST_HR(m_event, "Event creation failed");

    auto device = m_ctx->get_device();
    ANT_CHECK_HR(device->CreateFence(m_fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)), "Fence creation failed");

    D3D12_COMMAND_QUEUE_DESC qd;
    qd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    qd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    qd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    qd.NodeMask = 0;
    ANT_CHECK_HR(device->CreateCommandQueue(&qd, IID_PPV_ARGS(&m_queue)), "Failed to create command queue!");
}

ant::sdf::d3d_executor::~d3d_executor()
{
    flush();
}

bool ant::sdf::d3d_executor::check_cookie_completion(cookie cookie_value)
{
    if (cookie_value > m_last_completed)
    {
        m_last_completed = m_fence->GetCompletedValue();
        if (cookie_value > m_last_completed)
        {
            return false;
        }
    }
    return true;
}

void ant::sdf::d3d_executor::wait_for_cookie(cookie cookie_value)
{
    if (!check_cookie_completion(cookie_value))
    {
        m_fence->SetEventOnCompletion(cookie_value, m_event);
        ANT_CHECK(WaitForSingleObject(m_event, 5000) == WAIT_OBJECT_0, "Wait timeout");
        m_last_completed = m_fence->GetCompletedValue();
    }
}

void ant::sdf::d3d_executor::flush(size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        auto cookie_value = signal();
        wait_for_cookie(cookie_value);
    }
}

ant::sdf::d3d_executor::cookie ant::sdf::d3d_executor::signal()
{
    cookie cookie_value = ++m_fence_value;
    ANT_CHECK_HR(m_queue->Signal(m_fence, cookie_value), "Failed to place signal on queue");
    return cookie_value;
}

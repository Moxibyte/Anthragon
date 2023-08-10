#pragma once

#include <sdf-render/win.h>
#include <sdf-render/util/exception.h>
#include <sdf-render/d3d/d3d_context.h>

#include <memory>

namespace ant::sdf
{
    class d3d_executor
    {
        public:
            using ptr = std::shared_ptr<d3d_executor>;
            using cookie = size_t;

        public:
            d3d_executor() = delete;
            d3d_executor(d3d_context::ptr ctx);
            d3d_executor(const d3d_executor&) = delete;
            d3d_executor(d3d_executor&&) noexcept = delete;
            ~d3d_executor();

            d3d_executor& operator=(const d3d_executor&) = delete;
            d3d_executor& operator=(d3d_executor&&) noexcept = delete;

            template<typename... Args>
            cookie execute(Args... args)
            {
                ID3D12CommandList* lists[] = { args... };
                m_queue->ExecuteCommandLists(sizeof...(Args), lists);
                return signal();
            }

            bool check_cookie_completion(cookie cookie_value);
            void wait_for_cookie(cookie cookie_value);
            void flush(size_t count = 1);

            inline auto get_queue_ptr() { return m_queue; }
        private:
            cookie signal();

        private:
            d3d_context::ptr m_ctx;

            HANDLE m_event;
            cookie m_fence_value = 0;
            cookie m_last_completed = 0;

            comptr<ID3D12Fence1> m_fence;
            comptr<ID3D12CommandQueue> m_queue;
    };
}

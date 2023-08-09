#pragma once

#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_executor.h>

#include <vector>
#include <algorithm>

namespace ant::sdf
{
    class d3d_command_list
    {
        public:
            using ptr = std::shared_ptr<d3d_command_list>;

        public:
            d3d_command_list() = delete;
            d3d_command_list(d3d_context::ptr ctx);
            d3d_command_list(const d3d_command_list&) = delete;
            d3d_command_list(d3d_command_list&&) noexcept = delete;
            ~d3d_command_list();

            d3d_command_list& operator=(const d3d_command_list&) = delete;
            d3d_command_list& operator=(d3d_command_list&&) noexcept = delete;

            void add_resource_barrier(D3D12_RESOURCE_BARRIER& barr);
            void stage_resource_barriers();

            void execute_sync(d3d_executor::ptr executor);
            void execute_async(d3d_executor::ptr executor);
            void execute_async_wait();
            void reset();

            bool is_executing();

        private:
            void dispatch_executor();

        private:
            d3d_context::ptr m_ctx;

            d3d_executor::ptr m_active_executor = nullptr;
            d3d_executor::cookie m_completion_cookie = 0;

            comptr<ID3D12CommandAllocator> m_allocator;
            comptr<ID3D12GraphicsCommandList6> m_cmd_list;

            std::vector<D3D12_RESOURCE_BARRIER> m_barriers;
            size_t m_valid_barriers = 0;
    };
}

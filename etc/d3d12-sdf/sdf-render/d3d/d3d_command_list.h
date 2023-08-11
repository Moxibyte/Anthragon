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

            inline void copy_buffer(size_t size, ID3D12Resource* src, size_t src_offset, ID3D12Resource* dst, size_t dst_offset) 
            { 
                m_cmd_list->CopyBufferRegion(dst, dst_offset, src, src_offset, size); 
            }
            void copy_texture(ID3D12Resource* src, ID3D12Resource* dst, uint32_t width, uint32_t height, uint32_t bpp, DXGI_FORMAT format);

            template<typename... Args>
            void set_descriptor_heaps(Args... args)
            {
                ID3D12DescriptorHeap* heaps[] = { args... };
                m_cmd_list->SetDescriptorHeaps(sizeof...(Args), heaps);
            }

            void clear_rtv(D3D12_CPU_DESCRIPTOR_HANDLE rtv);
            inline void set_pso_rs_gfx(ID3D12PipelineState* pso, ID3D12RootSignature* rs) { m_cmd_list->SetPipelineState(pso); m_cmd_list->SetGraphicsRootSignature(rs); };
            inline void draw(size_t vertex_count) { m_cmd_list->DrawInstanced(vertex_count, 1, 0, 0); }
            inline void set_graphics_root_descriptor_table(size_t index, D3D12_GPU_DESCRIPTOR_HANDLE handle) { m_cmd_list->SetGraphicsRootDescriptorTable(index, handle); }
            inline void set_graphics_root_constant_buffer_view(size_t index, D3D12_GPU_VIRTUAL_ADDRESS address) { m_cmd_list->SetGraphicsRootConstantBufferView(index, address); }
            inline void set_graphics_root_32bit_constants(size_t index, size_t count, const void* data) { m_cmd_list->SetGraphicsRoot32BitConstants(index, count, data, 0);  }

            inline void ia_set_primitive_topology(D3D12_PRIMITIVE_TOPOLOGY topology) { m_cmd_list->IASetPrimitiveTopology(topology); }
            void ia_set_vertex_buffer(size_t size, size_t stride, D3D12_GPU_VIRTUAL_ADDRESS address);
            inline void rs_set_viewport(const D3D12_VIEWPORT& vp) { m_cmd_list->RSSetViewports(1, &vp); }
            inline void rs_set_sc_rect(const D3D12_RECT& scr) { m_cmd_list->RSSetScissorRects(1, &scr); }
            inline void om_set_rtv(const D3D12_CPU_DESCRIPTOR_HANDLE& rtv) { m_cmd_list->OMSetRenderTargets(1, &rtv, false, nullptr); }

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

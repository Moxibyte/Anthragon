#pragma once

#include <sdf-render/d3d/d3d_context.h>

namespace ant::sdf
{
    class d3d_descriptor_heap
    {
        public:
            using ptr = std::shared_ptr<d3d_descriptor_heap>;

        public:
            d3d_descriptor_heap() = delete;
            d3d_descriptor_heap(d3d_context::ptr ctx, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count, bool shader_visible = true);
            d3d_descriptor_heap(const d3d_descriptor_heap&) = delete;
            d3d_descriptor_heap(d3d_descriptor_heap&&) noexcept = delete;

            d3d_descriptor_heap& operator=(const d3d_descriptor_heap&) = delete;
            d3d_descriptor_heap& operator=(d3d_descriptor_heap&&) noexcept = delete;

            D3D12_CPU_DESCRIPTOR_HANDLE get_cpu(size_t index);
            D3D12_GPU_DESCRIPTOR_HANDLE get_gpu(size_t index);

        private:
            size_t m_count;
            D3D12_DESCRIPTOR_HEAP_TYPE m_type;
            comptr<ID3D12DescriptorHeap> m_heap;
            
            size_t m_increment = 0;
            D3D12_CPU_DESCRIPTOR_HANDLE m_handle_cpu{};
            D3D12_GPU_DESCRIPTOR_HANDLE m_handle_gpu{};
    };
}

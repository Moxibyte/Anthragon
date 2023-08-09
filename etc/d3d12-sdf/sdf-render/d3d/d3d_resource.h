#pragma once

#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_command_list.h>

#include <memory>
#include <functional>

namespace ant::sdf
{
    class d3d_resource
    {
        public:
            using ptr = std::shared_ptr<d3d_resource>;

        public:
            d3d_resource() = delete;
            d3d_resource(d3d_context::ptr ctx, size_t size, D3D12_HEAP_TYPE heap, D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state);
            d3d_resource(const d3d_resource&) = delete;
            d3d_resource(d3d_resource&&) noexcept = delete;

            d3d_resource& operator=(const d3d_resource&) = delete;
            d3d_resource& operator=(d3d_resource&&) noexcept = delete;

            void state_transition(d3d_command_list::ptr list, D3D12_RESOURCE_STATES state);

            inline size_t size() const { return m_size; }
            inline D3D12_RESOURCE_STATES state() const { return m_state; }

        public:
            static ptr create_upload_buffer(d3d_context::ptr ctx, size_t size);
            static ptr create_default_buffer(d3d_context::ptr ctx, size_t size);

        private:
            static ptr create_buffer(d3d_context::ptr ctx, size_t size, D3D12_HEAP_TYPE heap, D3D12_RESOURCE_STATES state);
            static size_t pad_size(size_t in, size_t pad);

        private:
            d3d_context::ptr m_ctx;
            comptr<ID3D12Resource2> m_resource;

            size_t m_size = 0;
            D3D12_HEAP_TYPE m_heap;
            D3D12_RESOURCE_STATES m_state;
    };
}

#pragma once

#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_resource.h>
#include <sdf-render/d3d/d3d_command_list.h>

namespace ant::sdf
{
    class d3d_uploader
    {
        public:
            using ptr = std::shared_ptr<d3d_uploader>;

        public:
            d3d_uploader() = delete;
            d3d_uploader(d3d_context::ptr ctx, size_t size);
            d3d_uploader(const d3d_uploader&) = delete;
            d3d_uploader(d3d_uploader&&) noexcept = delete;
            ~d3d_uploader();

            d3d_uploader& operator=(const d3d_uploader&) = delete;
            d3d_uploader& operator=(d3d_uploader&&) noexcept = delete;

            void reset();

            void stage_buffer(const void* data, size_t data_size, size_t target_offset, ID3D12Resource* resource, d3d_command_list::ptr command_list);
            void stage_texture(ID3D12Resource* resource, const void* data, uint32_t width, uint32_t height, uint32_t bpp, DXGI_FORMAT format, d3d_command_list::ptr command_list);

        private:
            void open();
            void close();

        private:
            d3d_resource::ptr m_upload_buffer;

            size_t m_write_head = 0;
            char* m_mapped_ptr = nullptr;
    };
}

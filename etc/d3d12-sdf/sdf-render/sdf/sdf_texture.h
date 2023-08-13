#pragma once

#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_resource.h>
#include <sdf-render/d3d/d3d_uploader.h>
#include <sdf-render/d3d/d3d_executor.h>
#include <sdf-render/d3d/d3d_command_list.h>

#include <vector>

namespace ant::sdf
{
    class sdf_texture
    {
        public:
            using ptr = std::shared_ptr<sdf_texture>;

        public:
            sdf_texture() = delete;
            sdf_texture(d3d_context::ptr ctx, d3d_executor::ptr executor, d3d_uploader::ptr uploader, d3d_command_list::ptr command_list, uint32_t width, uint32_t height, std::vector<float>& data);
            sdf_texture(const sdf_texture&) = delete;
            sdf_texture(sdf_texture&&) noexcept = delete;

            sdf_texture& operator=(const sdf_texture&) = delete;
            sdf_texture& operator=(sdf_texture&&) noexcept = delete;

            void create_srv(D3D12_SHADER_RESOURCE_VIEW_DESC& srv);

            inline ID3D12Resource* get_ptr() { return m_resource->get_ptr(); }

        private:
            d3d_resource::ptr m_resource;
    };
}


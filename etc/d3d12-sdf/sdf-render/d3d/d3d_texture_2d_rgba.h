#pragma once

#include <sdf-render/util/image.h>

#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_resource.h>
#include <sdf-render/d3d/d3d_uploader.h>
#include <sdf-render/d3d/d3d_executor.h>
#include <sdf-render/d3d/d3d_command_list.h>

#include <filesystem>

namespace ant::sdf
{
    class d3d_texture_2d_rgba
    {
        public:
            using ptr = std::shared_ptr<d3d_texture_2d_rgba>;

        public:
            d3d_texture_2d_rgba() = delete;
            d3d_texture_2d_rgba(d3d_context::ptr ctx, d3d_executor::ptr executor, d3d_uploader::ptr uploader, d3d_command_list::ptr command_list, const std::filesystem::path& path);
            d3d_texture_2d_rgba(const d3d_texture_2d_rgba&) = delete;
            d3d_texture_2d_rgba(d3d_texture_2d_rgba&&) noexcept = delete;

            d3d_texture_2d_rgba& operator=(const d3d_texture_2d_rgba&) = delete;
            d3d_texture_2d_rgba& operator=(d3d_texture_2d_rgba&&) noexcept = delete;

            void create_srv(D3D12_SHADER_RESOURCE_VIEW_DESC& srv);

            inline ID3D12Resource* get_ptr() { return m_resource->get_ptr(); }

        private:
            d3d_resource::ptr m_resource;
            image::ptr m_img;
    };
}

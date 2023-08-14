#pragma once

#include <sdf-render/sdf/sdf_layout.h>
#include <sdf-render/sdf/sdf_renderstate.h>
#include <sdf-render/sdf/sdf_rasterizer.h>
#include <sdf-render/sdf/sdf_texture.h>

#include <sdf-render/util/stb_rect_pack.h>

#include <array>
#include <queue>
#include <vector>
#include <algorithm>

#define ANT_SDF__NUM_SDF_DESCS 128
#define ANT_SDF__NUM_SDF_QUADS 512

namespace ant::sdf
{
    class sdf_renderer
    {
        public:
            using ptr = std::shared_ptr<sdf_renderer>;
            using sdf_index = size_t;
            using sdf_desc_slot = size_t;
            
            static constexpr sdf_index SDFID_always = 0;
            static constexpr sdf_index SDFID_never = 1;
            static constexpr sdf_index SDFID_circle = 2;
            static constexpr sdf_index SDFID_triangle = 3;

            struct sdf_desc
            {
                sdf_rasterizer::raster_function sdf;
                uint32_t raster_width;
                uint32_t raster_height;
            };

        private:
            struct sdf_temp
            {
                std::vector<float> data;
                uint32_t package_id;
                uint32_t offset_x;
                uint32_t offset_y;
            };

            struct sdf_meta
            {
                sdf_renderstate::texture_slot texture_id;

                float uv_tl[2];
                float uv_br[2];
            };

        public:
            sdf_renderer() = delete;
            sdf_renderer(d3d_context::ptr ctx);
            sdf_renderer(const sdf_renderer&) = delete;
            sdf_renderer(sdf_renderer&&) noexcept = delete;

            sdf_renderer& operator=(const sdf_renderer&) = delete;
            sdf_renderer& operator=(sdf_renderer&&) noexcept = delete;

            void process_sdfs(uint32_t textureSize = 128);
            void upload_sdf_data(d3d_executor::ptr executor, d3d_uploader::ptr uploader, d3d_command_list::ptr cmd_list);

            void bind(d3d_command_list::ptr cmd_list);
            void copy_data(d3d_command_list::ptr cmd_list, d3d_uploader::ptr uploader);
            void draw(d3d_command_list::ptr cmd_list);
            void set_aa_scaling_factor(float aa_scaling_factor);

            void stage_quad(float const pos[2], float const size[2], float const color[4], sdf_renderstate::texture_slot texture, sdf_desc_slot sdf_desc);
            void stage_ant_sdf_quad_pos(ant_sdf_quad_pos& quad);

            inline sdf_renderstate::texture_slot allocate_texture(ID3D12Resource* texture, D3D12_SHADER_RESOURCE_VIEW_DESC& texture_srv) { return m_state->allocate_texture(texture, texture_srv); }
            sdf_desc_slot allocate_sdf_desc(sdf_index sdf_function, float weight = 1.0f);

        private:
            void begin_copy(d3d_command_list::ptr cmd_list);
            void end_copy(d3d_command_list::ptr cmd_list);
            void copy_quads(d3d_command_list::ptr cmd_list, d3d_uploader::ptr uploader, ant_sdf_quad_pos* quads, size_t quad_count);

            bool sdf_pack_iteration(bool commit, int offset, int count, int size);

            void describe_sdf(sdf_desc_slot slot, sdf_index sdf_function, float weight);

        private:
            static float SDF_always(float x, float y);
            static float SDF_never(float x, float y);
            static float SDF_circle(float x, float y);
            static float SDF_triangle(float x, float y);

        private:
            d3d_context::ptr m_ctx;
            std::shared_ptr<sdf_renderstate> m_state;

            d3d_resource::ptr m_sdf_desc_buffer;
            d3d_resource::ptr m_sdf_quad_buffer;

            size_t m_quad_count = 0;
            std::array<ant_sdf_quad_pos, ANT_SDF__NUM_SDF_QUADS> m_quads;

            std::array<ant_sdf_desc, ANT_SDF__NUM_SDF_DESCS> m_descs;
            std::queue<sdf_desc_slot> m_desc_free_list;

            uint32_t m_sdf_texture_count = 0;
            uint32_t m_sdf_texture_size = 0;
            std::vector<sdf_desc> m_sdfs;
            std::vector<sdf_temp> m_sdf_cache;
            std::vector<sdf_meta> m_sdf_info;
            std::vector<sdf_texture::ptr> m_sdf_textures;
    };
}

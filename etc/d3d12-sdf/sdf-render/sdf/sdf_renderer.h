#pragma once

#include <sdf-render/sdf/sdf_layout.h>
#include <sdf-render/sdf/sdf_renderstate.h>

#include <array>

#define ANT_SDF__NUM_SDF_DESCS 128
#define ANT_SDF__NUM_SDF_QUADS 512

namespace ant::sdf
{
    class sdf_renderer
    {
        public:
            using ptr = std::shared_ptr<sdf_renderer>;

        public:
            sdf_renderer() = delete;
            sdf_renderer(d3d_context::ptr ctx);
            sdf_renderer(const sdf_renderer&) = delete;
            sdf_renderer(sdf_renderer&&) noexcept = delete;

            sdf_renderer& operator=(const sdf_renderer&) = delete;
            sdf_renderer& operator=(sdf_renderer&&) noexcept = delete;

            void bind(d3d_command_list::ptr cmd_list);
            void copy_data(d3d_command_list::ptr cmd_list, d3d_uploader::ptr uploader);
            void draw(d3d_command_list::ptr cmd_list);
            void set_aa_scaling_factor(float aa_scaling_factor);

            void stage_quad(float const pos[2], float const size[2], float const color[4], sdf_renderstate::texture_slot texture);
            void stage_ant_sdf_quad_pos(ant_sdf_quad_pos& quad);

            inline sdf_renderstate::texture_slot allocate_texture(ID3D12Resource* texture, D3D12_SHADER_RESOURCE_VIEW_DESC& texture_srv) { return m_state->allocate_texture(texture, texture_srv); }

        private:
            void begin_copy(d3d_command_list::ptr cmd_list);
            void end_copy(d3d_command_list::ptr cmd_list);
            void copy_quads(d3d_command_list::ptr cmd_list, d3d_uploader::ptr uploader, ant_sdf_quad_pos* quads, size_t quad_count);

        private:
            std::shared_ptr<sdf_renderstate> m_state;

            d3d_resource::ptr m_sdf_desc_buffer;
            d3d_resource::ptr m_sdf_quad_buffer;

            size_t m_quad_count = 0;
            std::array<ant_sdf_quad_pos, ANT_SDF__NUM_SDF_QUADS> m_quads;
    };
}

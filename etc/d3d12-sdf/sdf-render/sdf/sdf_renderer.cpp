#include "sdf_renderer.h"

ant::sdf::sdf_renderer::sdf_renderer(d3d_context::ptr ctx)
{
    m_state = std::make_shared<sdf_renderstate>(ctx);
    m_sdf_desc_buffer = d3d_resource::create_default_buffer(ctx, sizeof(ant_sdf_desc) * ANT_SDF__NUM_SDF_DESCS);
    m_sdf_quad_buffer = d3d_resource::create_default_buffer(ctx, sizeof(ant_sdf_quad_pos) * ANT_SDF__NUM_SDF_QUADS);
}

void ant::sdf::sdf_renderer::bind(d3d_command_list::ptr cmd_list)
{
    m_state->bind(cmd_list, m_sdf_desc_buffer);
}

void ant::sdf::sdf_renderer::copy_data(d3d_command_list::ptr cmd_list, d3d_uploader::ptr uploader)
{
    begin_copy(cmd_list);
    cmd_list->stage_resource_barriers();
    
    copy_quads(cmd_list, uploader, m_quads.data(), m_quad_count);
    
    end_copy(cmd_list);
    cmd_list->stage_resource_barriers();
}

void ant::sdf::sdf_renderer::draw(d3d_command_list::ptr cmd_list)
{
    if (m_quad_count)
    {
        cmd_list->ia_set_vertex_buffer(m_sdf_quad_buffer->size(), sizeof(ant_sdf_quad_pos), m_sdf_quad_buffer->gpu_address());
        cmd_list->draw(m_quad_count);
        m_quad_count = 0;
    }
}

void ant::sdf::sdf_renderer::stage_colored_quad(float const pos[2], float const size[2], float const color[4])
{
    ant_sdf_quad_pos quad{};
    quad.pos_tl[0] = pos[0];
    quad.pos_tl[1] = pos[1];
    quad.pos_br[0] = pos[0] + size[0];
    quad.pos_br[1] = pos[1] - size[1];
    quad.uv_tl[0] = 0.0f;
    quad.uv_tl[1] = 0.0f;
    quad.uv_br[0] = 1.f;
    quad.uv_br[1] = 1.f;
    quad.base_color[0] = color[0];
    quad.base_color[1] = color[1];
    quad.base_color[2] = color[2];
    quad.base_color[3] = color[3];

    stage_ant_sdf_quad_pos(quad);
}

void ant::sdf::sdf_renderer::stage_ant_sdf_quad_pos(ant_sdf_quad_pos& quad)
{
    ANT_CHECK(m_quad_count < ANT_SDF__NUM_SDF_QUADS, "Maximum number of quads exeeded");
    memcpy(&m_quads[m_quad_count++], &quad, sizeof(ant_sdf_quad_pos));
}

void ant::sdf::sdf_renderer::begin_copy(d3d_command_list::ptr cmd_list)
{
    m_sdf_desc_buffer->state_transition(cmd_list, D3D12_RESOURCE_STATE_COPY_DEST);
    m_sdf_quad_buffer->state_transition(cmd_list, D3D12_RESOURCE_STATE_COPY_DEST);
}

void ant::sdf::sdf_renderer::end_copy(d3d_command_list::ptr cmd_list)
{
    m_sdf_desc_buffer->state_transition(cmd_list, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    m_sdf_quad_buffer->state_transition(cmd_list, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
}

void ant::sdf::sdf_renderer::copy_quads(d3d_command_list::ptr cmd_list, d3d_uploader::ptr uploader, ant_sdf_quad_pos* quads, size_t quad_count)
{
    uploader->stage_buffer(quads, sizeof(ant_sdf_quad_pos) * quad_count, 0, m_sdf_quad_buffer->get_ptr(), cmd_list);
}

void ant::sdf::sdf_renderer::set_aa_scaling_factor(float aa_scaling_factor)
{
    m_state->set_aa_scaling(aa_scaling_factor);
}

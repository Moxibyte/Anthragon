#include "sdf_renderer.h"

ant::sdf::sdf_renderer::sdf_renderer(d3d_context::ptr ctx) :
    m_ctx(ctx)
{
    m_state = std::make_shared<sdf_renderstate>(ctx);
    m_sdf_desc_buffer = d3d_resource::create_default_buffer(ctx, sizeof(ant_sdf_desc) * ANT_SDF__NUM_SDF_DESCS);
    m_sdf_quad_buffer = d3d_resource::create_default_buffer(ctx, sizeof(ant_sdf_quad_pos) * ANT_SDF__NUM_SDF_QUADS);

    m_sdfs.push_back({ &sdf_renderer::SDF_always, 1, 1 });
    m_sdfs.push_back({ &sdf_renderer::SDF_never, 1, 1 });
    m_sdfs.push_back({ &sdf_renderer::SDF_circle, 32, 32 });
    m_sdfs.push_back({ &sdf_renderer::SDF_triangle, 32, 32 });
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
    uploader->stage_buffer(m_descs.data(), sizeof(ant_sdf_desc) * ANT_SDF__NUM_SDF_DESCS, 0, m_sdf_desc_buffer->get_ptr(), cmd_list);
    
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

void ant::sdf::sdf_renderer::stage_quad(float const pos[2], float const size[2], float const color[4], sdf_renderstate::texture_slot texture)
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
    quad.texture_id = texture;
    quad.sdf_desc_idx = 0;

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

void ant::sdf::sdf_renderer::process_sdfs(uint32_t textureSize)
{
    // Raster SDFs
    for (auto& sdf : m_sdfs)
    {
        sdf.raster_width = sdf_rasterizer::adjust_size(sdf.raster_width);
        sdf.raster_height = sdf_rasterizer::adjust_size(sdf.raster_height);

        sdf_temp temp;
        temp.data = sdf_rasterizer::raster(sdf.raster_width, sdf.raster_height, sdf.sdf);

        m_sdf_cache.push_back(std::move(temp));
    }

    // Rect pack SDFs
    size_t packed_count = 0;
    size_t itteration = 0;
    while (packed_count < m_sdfs.size())
    {
        size_t current = 1;
        while (sdf_pack_iteration(true, packed_count, current, textureSize))
        {
            if (packed_count + current == m_sdfs.size())
            {
                break;
            }
            current++;
        }
        for (size_t i = packed_count; i < packed_count + current; i++)
        {
            m_sdf_cache[i].package_id = itteration;
        }

        packed_count += current;
        itteration++;
    }   

    m_sdf_texture_count = itteration;
    m_sdf_texture_size = textureSize;
}

bool ant::sdf::sdf_renderer::sdf_pack_iteration(bool commit, int offset, int count, int size)
{
    stbrp_context ctx;

    std::vector<stbrp_rect> rects(count);
    std::vector<stbrp_node> nodes(count);
    for (int i = 0; i < count; i++)
    {
        rects[i].id = offset + i;
        rects[i].w = m_sdfs[offset + i].raster_width;
        rects[i].h = m_sdfs[offset + i].raster_height;
    }

    stbrp_init_target(&ctx, size, size, nodes.data(), count);
    if (stbrp_pack_rects(&ctx, rects.data(), count) == 1)
    {
        if (commit)
        {
            for (size_t i = 0; i < count; i++)
            {
                auto idx = rects[i].id;
                m_sdf_cache[idx].offset_x = rects[i].x;
                m_sdf_cache[idx].offset_y = rects[i].y;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

void ant::sdf::sdf_renderer::upload_sdf_data(d3d_executor::ptr executor, d3d_uploader::ptr uploader, d3d_command_list::ptr cmd_list)
{
    std::vector<std::vector<float>> texture_data(m_sdf_texture_count);
    m_sdf_info.resize(m_sdfs.size());

    for (size_t i = 0; i < m_sdf_texture_count; i++)
    {
        texture_data[i].resize(m_sdf_texture_size * m_sdf_texture_size, NAN);
        std::vector<size_t> idxs;

        for (size_t sdf_i = 0; sdf_i < m_sdfs.size(); sdf_i++)
        {
            auto& sdf = m_sdf_cache[sdf_i];
            if (sdf.package_id == i)
            {
                auto& sdfd = m_sdfs[sdf_i];
                auto& sdfi = m_sdf_info[sdf_i];
                
                sdf_rasterizer::bit_blend(texture_data[i], m_sdf_texture_size, m_sdf_texture_size, sdf.offset_x, sdf.offset_y, sdf.data, sdfd.raster_width, sdfd.raster_height);
                idxs.push_back(sdf_i);
            }
        }

        auto texture = std::make_shared<sdf_texture>(m_ctx, executor, uploader, cmd_list, m_sdf_texture_size, m_sdf_texture_size, texture_data[i]);
        m_sdf_textures.push_back(texture);

        D3D12_SHADER_RESOURCE_VIEW_DESC srv;
        texture->create_srv(srv);

        const float dxy = 1.0f / (m_sdf_texture_size - 1);

        auto slot_id = allocate_texture(texture->get_ptr(), srv);
        for (size_t idx : idxs)
        {
            m_sdf_info[idx].texture_id = slot_id;
            m_sdf_info[idx].uv_tl[0] = m_sdf_cache[idx].offset_x * dxy;
            m_sdf_info[idx].uv_tl[1] = m_sdf_cache[idx].offset_y * dxy;
            m_sdf_info[idx].uv_br[0] = (m_sdf_cache[idx].offset_x + m_sdfs[idx].raster_width - 1) * dxy;
            m_sdf_info[idx].uv_br[1] = (m_sdf_cache[idx].offset_y + m_sdfs[idx].raster_height - 1) * dxy;
        }
    }

    m_sdfs.clear();
    m_sdf_cache.clear();

    const size_t id = 2;
    m_descs[0].texture_id = m_sdf_info[id].texture_id;
    m_descs[0].texture_weight = 1.0f;
    memcpy(&m_descs[0].uv_tl, &m_sdf_info[id].uv_tl, sizeof(float) * 2);
    memcpy(&m_descs[0].uv_br, &m_sdf_info[id].uv_br, sizeof(float) * 2);
}

float ant::sdf::sdf_renderer::SDF_always(float x, float y)
{
    return 0.0f;
}

float ant::sdf::sdf_renderer::SDF_never(float x, float y)
{
    return -1.0f;
}

float ant::sdf::sdf_renderer::SDF_circle(float x, float y)
{
    return 1.0f - sqrtf(x * x + y * y);
}

float ant::sdf::sdf_renderer::SDF_triangle(float x, float y)
{
    /*
    static const float k = sqrt(3.0);
    static const float r = 1.0f;

    x = abs(x) - r;
    y = y + r / k;
    if (x + k * y > 0.0)
    {
        x = x - k * y;
        y = -1 * k * x - y;
        x /= 2;
        y /= 2;
    }
    x -= std::clamp<float>(x, -2.0 * r, 0.0);
    return -1 * sqrtf(x * x + y * y) * (y > 0 ? 1 : -1);
    */

    x = fabs(x);
    y = (1.0f + y) / 2;
    float d = fabs(-1.0f * x + y + 1.0f) / 1.41421356f;

    return 1.0f - d;
}

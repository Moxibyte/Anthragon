#include "sdf_renderer.h"

ant::sdf::sdf_renderer::sdf_renderer(d3d_context::ptr ctx) :
    m_ctx(ctx)
{
    m_state = std::make_shared<sdf_renderstate>(ctx);
    m_sdf_desc_buffer = d3d_resource::create_default_buffer(ctx, sizeof(ant_sdf_desc) * ANT_SDF__NUM_SDF_DESCS);
    m_sdf_quad_buffer = d3d_resource::create_default_buffer(ctx, sizeof(ant_sdf_quad_pos) * ANT_SDF__NUM_SDF_QUADS);

    m_sdfs.push_back({ &sdf_renderer::SDF_always, 1, 1 });
    m_sdfs.push_back({ &sdf_renderer::SDF_never, 1, 1 });
    m_sdfs.push_back({ &sdf_renderer::SDF_circle, 17, 17 });
    m_sdfs.push_back({ &sdf_renderer::SDF_triangle, 17, 17 });
    m_sdfs.push_back({ &sdf_renderer::SDF_ring, 31, 31 });
    m_sdfs.push_back({ &sdf_renderer::SDF_line, 5, 5 });

    for (size_t i = 0; i < ANT_SDF__NUM_SDF_DESCS; i += 8)
    {
        m_desc_free_list.push(i);
    }
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

void ant::sdf::sdf_renderer::quad_atl(float const pos[2], float const size[2], float rotation, float const color[4], sdf_renderstate::texture_slot texture, sdf_desc_slot sdf_desc)
{
    float cror = rotation + 3.1415926535897932384626433832795f * 1.5f;

    ant_sdf_quad_pos quad{};
    quad.pos[0] = pos[0];
    quad.pos[1] = pos[1];
    quad.span[0] = 0.0f;
    quad.span[1] = 0.0f;
    quad.span[2] = cosf(cror)* size[0] - sinf(cror) * (-1.f * size[1]);
    quad.span[3] = sinf(cror)* size[0] + cosf(cror) * (-1.f * size[1]);
    quad.uv_tl[0] = 0.f;
    quad.uv_tl[1] = 0.f;
    quad.uv_br[0] = 1.f;
    quad.uv_br[1] = 1.f;
    quad.base_color[0] = color[0];
    quad.base_color[1] = color[1];
    quad.base_color[2] = color[2];
    quad.base_color[3] = color[3];
    quad.texture_id = texture;
    quad.sdf_desc_idx = sdf_desc;

    stage_ant_sdf_quad_pos(quad);
}

void ant::sdf::sdf_renderer::stage_ant_sdf_quad_pos(ant_sdf_quad_pos& quad)
{
    ANT_CHECK(m_quad_count < ANT_SDF__NUM_SDF_QUADS, "Maximum number of quads exeeded");
    memcpy(&m_quads[m_quad_count++], &quad, sizeof(ant_sdf_quad_pos));
}

ant::sdf::sdf_renderer::sdf_desc_slot ant::sdf::sdf_renderer::allocate_sdf_desc(sdf_index sdf_function, float offset)
{
    if (!m_desc_free_list.empty())
    {
        auto slot = m_desc_free_list.front();
        m_desc_free_list.pop();

        describe_sdf(slot, sdf_function, offset);
        for (size_t i = 1; i < 8; i++)
        {
            describe_sdf(slot + i, SDFID_never, 0.0f);
        }

        return slot;
    }
    return -1;
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

void ant::sdf::sdf_renderer::set_aspect_ratio(float aspect_ratio)
{
    m_state->set_aspect_ratio(aspect_ratio);
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
        rects[i].w = m_sdfs[offset + i].raster_width + sdf_rasterizer::OVERSCAN * 2;
        rects[i].h = m_sdfs[offset + i].raster_height + sdf_rasterizer::OVERSCAN * 2;
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

void ant::sdf::sdf_renderer::describe_sdf(sdf_desc_slot slot, sdf_index sdf_function, float offset)
{
    m_descs[slot].texture_id = m_sdf_info[sdf_function].texture_id;
    m_descs[slot].offset = offset;
    memcpy(&m_descs[slot].uv_tl, &m_sdf_info[sdf_function].uv_tl, sizeof(float) * 2);
    memcpy(&m_descs[slot].uv_br, &m_sdf_info[sdf_function].uv_br, sizeof(float) * 2);
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

        const float dxy = 1.0f / m_sdf_texture_size;

        auto slot_id = allocate_texture(texture->get_ptr(), srv);
        for (size_t idx : idxs)
        {
            m_sdf_info[idx].texture_id = slot_id;
            m_sdf_info[idx].uv_tl[0] = (m_sdf_cache[idx].offset_x + sdf_rasterizer::OVERSCAN) * dxy;
            m_sdf_info[idx].uv_tl[1] = (m_sdf_cache[idx].offset_y + sdf_rasterizer::OVERSCAN) * dxy;
            m_sdf_info[idx].uv_br[0] = (m_sdf_cache[idx].offset_x + sdf_rasterizer::OVERSCAN + m_sdfs[idx].raster_width) * dxy;
            m_sdf_info[idx].uv_br[1] = (m_sdf_cache[idx].offset_y + sdf_rasterizer::OVERSCAN + m_sdfs[idx].raster_height) * dxy;
        }
    }

    m_sdfs.clear();
    m_sdf_cache.clear();
}

float ant::sdf::sdf_renderer::SDF_always(float x, float y)
{
    return 1.0f;
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
    x = fabs(x);                                    // Range was "-1 to 1" NOW ITS: "0 to 1" (Right half of triangle)
    y = (1.0f + y) / 2;                             // Range was "-1 to 1" NOW ITS: "(0 to 2) / 2" THAT IS: "0 to 1" (Rescale of half triangle)
    return (-1.0f * x - y + 1.0f) / 1.41421356f;    // Signed distance between P(x,y) and the line "y = -1x + 1" THAT IS "0 = -1x - y + 1"
}

float ant::sdf::sdf_renderer::SDF_ring(float x, float y)
{
    float d = 0.8f - sqrtf(x * x + y * y);
    return -1.0f * fabs(d);
}

float ant::sdf::sdf_renderer::SDF_line(float x, float y)
{
    return -1.0f * fabs(y);
}

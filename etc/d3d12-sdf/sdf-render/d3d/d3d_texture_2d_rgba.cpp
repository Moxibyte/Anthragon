#include "d3d_texture_2d_rgba.h"

ant::sdf::d3d_texture_2d_rgba::d3d_texture_2d_rgba(d3d_context::ptr ctx, d3d_executor::ptr executor, d3d_uploader::ptr uploader, d3d_command_list::ptr command_list, const std::filesystem::path& path)
{
    m_img = std::make_shared<image>(path);
    m_resource = d3d_resource::create_texture2d(ctx, m_img->get_width(), m_img->get_height(), m_img->get_format(), D3D12_RESOURCE_STATE_COPY_DEST);
    
    uploader->stage_texture(m_resource->get_ptr(), m_img->get_data(), m_img->get_width(), m_img->get_height(), m_img->get_bpp(), m_img->get_format(), command_list);
    m_resource->state_transition(command_list, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    command_list->execute_sync(executor);
    command_list->reset();
    uploader->reset();

    m_img->evict();
}

void ant::sdf::d3d_texture_2d_rgba::create_srv(D3D12_SHADER_RESOURCE_VIEW_DESC& srv)
{
    srv.Format = m_img->get_format();
    srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Texture2D.MostDetailedMip = 0;
    srv.Texture2D.MipLevels = 1;
    srv.Texture2D.PlaneSlice = 0;
    srv.Texture2D.ResourceMinLODClamp = 0.0f;
}

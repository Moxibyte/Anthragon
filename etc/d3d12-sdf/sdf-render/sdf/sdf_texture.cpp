#include "sdf_texture.h"

ant::sdf::sdf_texture::sdf_texture(d3d_context::ptr ctx, d3d_executor::ptr executor, d3d_uploader::ptr uploader, d3d_command_list::ptr command_list, uint32_t width, uint32_t height, std::vector<float>& data)
{
    m_resource = d3d_resource::create_texture2d(ctx, width, height, DXGI_FORMAT_R32_FLOAT, D3D12_RESOURCE_STATE_COPY_DEST);

    uploader->stage_texture(m_resource->get_ptr(), data.data(), width, height, 32, DXGI_FORMAT_R32_FLOAT, command_list);
    m_resource->state_transition(command_list, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    command_list->execute_sync(executor);
    command_list->reset();
    uploader->reset();
}

void ant::sdf::sdf_texture::create_srv(D3D12_SHADER_RESOURCE_VIEW_DESC& srv)
{
    srv.Format = DXGI_FORMAT_R32_FLOAT;
    srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Texture2D.MostDetailedMip = 0;
    srv.Texture2D.MipLevels = 1;
    srv.Texture2D.PlaneSlice = 0;
    srv.Texture2D.ResourceMinLODClamp = 0.0f;
}

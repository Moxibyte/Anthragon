#include "d3d_uploader.h"

ant::sdf::d3d_uploader::d3d_uploader(d3d_context::ptr ctx, size_t size)
{
    m_upload_buffer = d3d_resource::create_upload_buffer(ctx, size);
    open();
}

ant::sdf::d3d_uploader::~d3d_uploader()
{
    close();
}

void ant::sdf::d3d_uploader::reset()
{
    m_write_head = 0;
}

void ant::sdf::d3d_uploader::stage_buffer(const void* data, size_t data_size, size_t target_offset, ID3D12Resource* resource, d3d_command_list::ptr command_list)
{
    ANT_CHECK(m_mapped_ptr, "Upload buffer not mapped");
    ANT_CHECK(m_write_head + data_size <= m_upload_buffer->size(), "Insufficent memory allocated in upload buffer");

    memcpy(&m_mapped_ptr[m_write_head], data, data_size);
    command_list->copy_buffer(data_size, m_upload_buffer->get_ptr(), m_write_head, resource, target_offset);

    m_write_head += data_size;
}

void ant::sdf::d3d_uploader::open()
{
    m_mapped_ptr = (char*)m_upload_buffer->map();
}

void ant::sdf::d3d_uploader::close()
{
    if (m_mapped_ptr)
    {
        m_upload_buffer->unmap();
        m_mapped_ptr = nullptr;
    }
}

void ant::sdf::d3d_uploader::stage_texture(ID3D12Resource* resource, const void* data, uint32_t width, uint32_t height, uint32_t bpp, DXGI_FORMAT format, d3d_command_list::ptr command_list)
{
    ANT_CHECK(m_mapped_ptr, "Upload buffer not mapped");
    
    size_t data_size = height * (width * ((bpp + 7) / 8));
    ANT_CHECK(m_write_head + data_size <= m_upload_buffer->size(), "Insufficent memory allocated in upload buffer");

    memcpy(&m_mapped_ptr[m_write_head], data, data_size);
    command_list->copy_texture(m_upload_buffer->get_ptr(), resource, width, height, bpp, format);

    m_write_head += data_size;
}

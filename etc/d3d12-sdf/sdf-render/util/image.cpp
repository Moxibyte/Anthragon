#include "image.h"

const std::vector<ant::sdf::image::guid_to_dxgi_t> ant::sdf::image::guid_to_dxgi =
{
    { GUID_WICPixelFormat32bppBGR, DXGI_FORMAT_B8G8R8X8_UNORM },
    { GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM },
    { GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM },

    { GUID_WICPixelFormat32bppGrayFloat, DXGI_FORMAT_R32_FLOAT },
};

ant::sdf::image::image(const std::filesystem::path& img_path)
{
    // Create factory
    comptr<IWICImagingFactory> wic_factory;
    ANT_CHECK_HR(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_factory)), "Failed to create wic factory!");

    // Open image
    comptr<IWICStream> wic_stream;
    ANT_CHECK_HR(wic_factory->CreateStream(&wic_stream), "Failed to create input stream");
    ANT_CHECK_HR(wic_stream->InitializeFromFilename(img_path.wstring().c_str(), GENERIC_READ), "Failed to open file");

    comptr<IWICBitmapDecoder> wic_decoder;
    ANT_CHECK_HR(wic_factory->CreateDecoderFromStream(wic_stream, nullptr, WICDecodeMetadataCacheOnDemand, &wic_decoder), "Failed to initialize decoder from stream");

    comptr<IWICBitmapFrameDecode> wic_frame_decoder;
    ANT_CHECK_HR(wic_decoder->GetFrame(0, &wic_frame_decoder), "Failed to get frame from decoder");

    // Size
    ANT_CHECK_HR(wic_frame_decoder->GetSize(&m_width, &m_height), "Failed to retrive resolution");
    
    // WIC Format
    GUID pixel_format;
    ANT_CHECK_HR(wic_frame_decoder->GetPixelFormat(&pixel_format), "Failed to retrive pixel format");

    // DXGI Format
    auto it_find = std::find_if(guid_to_dxgi.begin(), guid_to_dxgi.end(), [&](const guid_to_dxgi_t& e) { return memcmp(&pixel_format, &e.guid, sizeof(GUID)) == 0; });
    ANT_CHECK(it_find != guid_to_dxgi.end(), "Pixel format of image not supported");
    m_format = it_find->dxgi;

    // BPP
    comptr<IWICComponentInfo> wic_component_info;
    ANT_CHECK_HR(wic_factory->CreateComponentInfo(pixel_format, &wic_component_info), "Failed to create component info from GUID pixel format");
    comptr<IWICPixelFormatInfo> wic_pixel_format;
    ANT_CHECK_HR(wic_component_info->QueryInterface(&wic_pixel_format), "Failed to convert component info to pixel format info");

    ANT_CHECK_HR(wic_pixel_format->GetBitsPerPixel(&m_bpp), "Failed to retrive bits per pixel");
    ANT_CHECK_HR(wic_pixel_format->GetChannelCount(&m_channel_count), "Failed to retrive number of channels");

    // Compute metrics
    auto stride = m_width * ((m_bpp + 7) / 8);
    auto size = stride * m_height;

    // Copy data
    m_data.resize(size);
    WICRect copy_rect;
    copy_rect.X = copy_rect.Y = 0;
    copy_rect.Width = m_width;
    copy_rect.Height = m_height;
    ANT_CHECK_HR(wic_frame_decoder->CopyPixels(&copy_rect, stride, size, (BYTE*)m_data.data()), "Failed to copy pixel");
}

void ant::sdf::image::evict()
{
    m_data.resize(1);
}

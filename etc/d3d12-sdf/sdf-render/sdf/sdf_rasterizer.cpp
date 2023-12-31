#include "sdf_rasterizer.h"

std::vector<float> ant::sdf::sdf_rasterizer::raster(uint32_t width, uint32_t height, raster_function sdf)
{
    std::vector<float> pixeldata((width + OVERSCAN * 2) * (height + OVERSCAN * 2), NAN);
    
    const float dx = 2.0f / (width - 1);
    const float dy = 2.0f / (height - 1);

    for (uint32_t y = 0; y < (height + OVERSCAN * 2); y++)
    {
        for (uint32_t x = 0; x < (width + OVERSCAN * 2); x++)
        {
            float ndc_x = (-1.0f - OVERSCAN * dx) + (x * dx);
            float ndc_y = (1.0f + OVERSCAN * dy) - (y * dy);
            pixeldata[(width + OVERSCAN * 2) * y + x] = sdf(ndc_x, ndc_y);
        }
    }

    return pixeldata;
}

void ant::sdf::sdf_rasterizer::bit_blend(std::vector<float>& dst, uint32_t dst_width, uint32_t dst_height, uint32_t dst_x, uint32_t dst_y, const std::vector<float>& src, uint32_t src_width, uint32_t src_height)
{
    const uint32_t dstr = dst_width;
    const uint32_t doff = dstr * dst_y + dst_x;

    for (uint32_t y = 0; y < (src_width + OVERSCAN * 2); y++)
    {
        for (uint32_t x = 0; x < (src_height + OVERSCAN * 2); x++)
        {
            dst[doff + y * dstr + x] = src[(src_width + OVERSCAN * 2) * y + x];
        }
    }
}

uint32_t ant::sdf::sdf_rasterizer::adjust_size(uint32_t wh)
{
    return wh % 2 == 0 ? wh + 1 : wh;
}

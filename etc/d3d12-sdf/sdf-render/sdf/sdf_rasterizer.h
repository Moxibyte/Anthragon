#pragma once

#include <vector>
#include <functional>

namespace ant::sdf
{
    class sdf_rasterizer
    {
        public:
            // In: x/y in NDC (-1.0f ... 0.0f ... 1.0f)
            // Out: Distance (- --> Outside; + --> Inside; 0 On edge
            using raster_function = std::function<float(float ndc_x, float ndc_y)>;
            
        public:
            static uint32_t adjust_size(uint32_t wh);
            static std::vector<float> raster(uint32_t width, uint32_t height, raster_function sdf);
            static void bit_blend(std::vector<float>& dst, uint32_t dst_width, uint32_t dst_height, uint32_t dst_x, uint32_t dst_y, const std::vector<float>& src, uint32_t src_width, uint32_t src_height);

        private:
            sdf_rasterizer() = delete;
            sdf_rasterizer(const sdf_rasterizer&) = delete;
            sdf_rasterizer(sdf_rasterizer&&) noexcept = delete;
    };
}

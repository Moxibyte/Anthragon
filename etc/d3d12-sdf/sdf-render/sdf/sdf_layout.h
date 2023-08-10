#pragma once

#include <cstdint>

namespace ant::sdf
{
    struct ant_sdf_quad_pos
    {
        float pos_tl[2];
        float pos_br[2];
        float uv_tl[2];
        float uv_br[2];
        float base_color[4];
        uint32_t sdf_desc_idx;
        uint32_t texture_id;
    };

    struct ant_sdf_desc
    {
        uint32_t sdf_textures[8];
        float sdf_weights[8];
    };
}

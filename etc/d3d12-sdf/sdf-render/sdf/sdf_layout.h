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
        uint32_t texture_id;   // 0
        float texture_weight;  // 4
        float uv_tl[2];        // 8
        float uv_br[2];        // 16
        char pad[8];           // 24
                               // 32 (END)
    };
}

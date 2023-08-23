#pragma once

#include <cstdint>

namespace ant::sdf
{
    struct ant_sdf_quad_pos
    {
        float pos[2];
        uint32_t sdf_desc_idx;
        uint32_t texture_id;
        float span[4];
        float uv_tl[2];
        float uv_br[2];
        float base_color[4];
    };

    struct ant_sdf_desc
    {
        uint32_t texture_id;   // 0
        float offset;          // 4
        float uv_tl[2];        // 8
        float uv_br[2];        // 16
        char pad[8];           // 24
                               // 32 (END)
    };

    struct ant_sdf_config
    {
        float aa_factor = 2.5f;
        float aspect_ratio = 1.0f;
        float pad[2];
    };
}

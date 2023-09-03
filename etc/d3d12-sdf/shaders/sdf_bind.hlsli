#ifndef SDF_BIND_H
#define SDF_BIND_H

#include "sdf_pipeline.hlsli"

// === Bindings ===
Texture2D<float4>   ANT_SFD_color_tex[]     : register(t0, space0);
Texture2D<float>    ANT_SFD_sdf_tex[]       : register(t0, space1);
sampler             ANT_SDF_image_sampler   : register(s0);
sampler             ANT_SDF_sdf_sampler     : register(s1);
cbuffer             ANT_SDF_sdf_descs       : register(b1)
{
    ant_sdf_desc ANT_SDF_sdf_descs_e[1];
}
cbuffer ANT_SDF_config : register(b0)
{
    float           ANT_SDF_aa_scaling;
    float           ANT_SDF_aspect_ratio;
}

#endif

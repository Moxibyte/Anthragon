#ifndef SDF_BIND_H
#define SDF_BIND_H

#include "sdf_pipeline.hlsli"

// === Bindings ===
Texture2D<float4>   ANT_SFD_textures[]      : register(t0);
ant_sdf_desc        ANT_SDF_sdf_descs[0]    : register(b1);
sampler             ANT_SDF_image_sampler   : register(s0);
sampler             ANT_SDF_sdf_sampler     : register(s1);

float               ANT_SDF_aa_scaling      : register(b0);

#endif

#ifndef SDF_PIPELINE_H
#define SDF_PIPELINE_H

// === Root signature === 
#define ANT_SDF_ROOTSIG \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
"DescriptorTable(" \
    "SRV(t0, space=0, numDescriptors=unbounded, flags=DESCRIPTORS_VOLATILE)" \
")," \
"DescriptorTable(" \
    "SRV(t0, space=1, numDescriptors=unbounded, flags=DESCRIPTORS_VOLATILE)" \
")," \
"CBV(b1, flags=DATA_VOLATILE)," \
"DescriptorTable(" \
    "Sampler(s0, numDescriptors = 2)" \
")," \
"RootConstants(num32BitConstants=1, b0)"

/*
*   CPU: 
*   - 0         : Textures SRVs (TABEL)
*   - 1         : Textures SRVs (TABEL)
*   - 2         : SDF Descriptors (CBV to arrays in memory)
*   - 3         : Samplers (TABEL)
*               : [0] Image
*               : [1] SDF
*   - 4         : AA-Scaling Factor (float)
*   
*   GPU:
*   - t0...     : Texture SRVs (space 0 & 1)
*   - b1        : SDF Descriptors
*   - s0        : Image sampler
*   - s1        : SDF sampler
*   - b0        : AA-ScalingFactor
*/

// === Memory primitives ===
struct ant_sdf_desc
{
    uint texture_id;
    float offset;
    float2 uv_tl;
    float2 uv_br;
};

// === Pipeline ABI ===
// point format
struct ant_sdf_quad_pos
{
    float2 pos_tl       : TopLeft;
    float2 pos_br       : BottomRight;
    float2 uv_tl        : TopLeftUV;
    float2 uv_br        : BottomRightUV;
    float4 base_color   : Color;
    uint   sdf_desc_idx : SDFDesc;
    uint   texture_id   : Texture;
};

// triangle format
struct ant_sdf_vertex
{
    float4 pos          : SV_Position;
    float4 color        : Color;
    float2 uv           : TexcoordUV; // Texture
    float2 ux           : TexcoordUX; // SDF
    float2 us           : TexcoordUS; // NDCs
    uint   sdf_desc_idx : SDFDesc;
    uint   texture_id   : Texture;
};

// pixel format
struct ant_sdf_pixel
{
    float4 color        : SV_Target;
    // float  depth        : SV_Depth;
};



#endif

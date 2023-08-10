#ifndef SDF_PIPELINE_H
#define SDF_PIPELINE_H

// === Root signature === 
#define ANT_SDF_ROOTSIG \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
"DescriptorTable(" \
    "SRV(t0, numDescriptors=unbounded, flags=DESCRIPTORS_VOLATILE)" \
")," \
"CBV(b0, flags=DATA_STATIC_WHILE_SET_AT_EXECUTE)," \
"DescriptorTable(" \
    "Sampler(s0, numDescriptors = 2)" \
")"

/*
*   CPU: 
*   - 0         : Textures SRVs (TABEL)
*   - 1         : SDF Descriptors (CBV to arrays in memory)
*   - 2         : Samplers (TABEL)
*               : [0] Image
*               : [1] SDF
*   
*   GPU:
*   - t0...     : Texture SRVs
*   - b0...     : SDF Descriptors
*   - s0        : Image sampler
*   - s1        : SDF sampler
*/

// === Memory primitives ===
struct ant_sdf_desc
{
    uint sdf_textures[8];
    float sdf_weights[8];
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
    float2 uv           : TexcoordUV;
    float2 us           : TexcoordUS;
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

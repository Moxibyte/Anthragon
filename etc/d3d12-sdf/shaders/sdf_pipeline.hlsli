#ifndef SDF_PIPELINE_H
#define SDF_PIPELINE_H

#define ANT_SDF__MAX_SDF_TEXTURES 8

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
"RootConstants(num32BitConstants=2, b0)"

/*
*   CPU: 
*   - 0         : Textures SRVs (TABEL)
*   - 1         : Textures SRVs (TABEL)
*   - 2         : SDF Descriptors (CBV to arrays in memory)
*   - 3         : Samplers (TABEL)
*               : [0] Image
*               : [1] SDF
*   - 4         : AA-Scaling Factor (float)
*   - 5         : Aspect ratio (float)
*   
*   GPU:
*   - t0...     : Texture SRVs (space 0 & 1)
*   - b2        : SDF Descriptors
*   - s0        : Image sampler
*   - s1        : SDF sampler
*   - b0        : AA-ScalingFactor
*   - b1        : Aspect ration correction 
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
    float2 pos          : Position;         // Anchor / Point
    uint   sdf_desc_idx : SDFDesc;          // SDF
    uint   texture_id   : Texture;          // TEXTURE
    
    float4 span         : Span;             // Point to quad expansion (lx, ly, rx, ry)
    
    float2 uv_tl        : TopLeftUV;        // UV (TEXTURE)
    float2 uv_br        : BottomRightUV;    // UV (TEXTURE)
    
    float4 base_color   : Color;            // Base color to multiply with texture
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

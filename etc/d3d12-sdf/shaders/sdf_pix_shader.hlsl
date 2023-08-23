#include "sdf_pipeline.hlsli"
#include "sdf_bind.hlsli"

float ant_sdf_sample(float2 ux, uint idx)
{
    float2 uv = lerp(ANT_SDF_sdf_descs_e[idx].uv_tl, ANT_SDF_sdf_descs_e[idx].uv_br, ux);
    
    uint sdf_texture_id = ANT_SDF_sdf_descs_e[idx].texture_id;
    return ANT_SFD_sdf_tex[sdf_texture_id].Sample(ANT_SDF_sdf_sampler, uv) + ANT_SDF_sdf_descs_e[idx].offset;
}

float ant_sdf_sample_all(float2 ux, uint idx)
{
    float d = ant_sdf_sample(ux, idx);
    [unroll] for(uint i = 1; i < 8; i++)
    {
        d = max(d, ant_sdf_sample(ux, idx + i));
    }
    return d;
}

[RootSignature(ANT_SDF_ROOTSIG)]
void main(in ant_sdf_vertex pxd, out ant_sdf_pixel px)
{
    // Compute distance via sdf (This should be some texture lookup later)
    float d = ant_sdf_sample_all(pxd.ux, pxd.sdf_desc_idx);

    // Create AA factor with the use of ddx/ddy
    float r = lerp(abs(ddx_coarse(pxd.us.x)), abs(ddy_coarse(pxd.us.y)), abs(pxd.us.y));
    
    // Calculate the alpha scaling with use of AA + AA-Scaling 
    float s = smoothstep(0.0f, r * ANT_SDF_aa_scaling, d);
    
    // Write the final color (Should be texture lookup later)
    px.color = float4(pxd.color.rgb * ANT_SFD_color_tex[pxd.texture_id].Sample(ANT_SDF_image_sampler, pxd.uv).rgb, s);
}

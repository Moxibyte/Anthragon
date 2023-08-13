#include "sdf_pipeline.hlsli"
#include "sdf_bind.hlsli"

[RootSignature(ANT_SDF_ROOTSIG)]
void main(in ant_sdf_vertex pxd, out ant_sdf_pixel px)
{
    // Compute distance via sdf (This should be some texture lookup later)
    uint sdf_idx = pxd.sdf_desc_idx;
    uint sdf_tex_idx = ANT_SDF_sdf_descs_e[sdf_idx].texture_id;
    float d = ANT_SFD_sdf_tex[sdf_tex_idx].Sample(ANT_SDF_sdf_sampler, pxd.ux);

    // Create AA factor with the use of ddx/ddy
    float r = lerp(ddx(pxd.us.x), abs(ddy(pxd.us.y)), abs(pxd.us.y));
    
    // Calculate the alpha scaling with use of AA + AA-Scaling 
    float s = smoothstep(0.0f, r * ANT_SDF_aa_scaling, d);
    
    // Write the final color (Should be texture lookup later)
    px.color = float4(pxd.color.rgb * ANT_SFD_color_tex[pxd.texture_id].Sample(ANT_SDF_image_sampler, pxd.uv).rgb, s);
    
    // px.color = float4(d, d, d, 1.0f);
    // px.color = float4(pxd.ux, 0.0f, 1.0f);
}

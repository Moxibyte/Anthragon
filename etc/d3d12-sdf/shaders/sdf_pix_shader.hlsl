#include "sdf_pipeline.hlsli"
#include "sdf_bind.hlsli"

[RootSignature(ANT_SDF_ROOTSIG)]
void main(in ant_sdf_vertex pxd, out ant_sdf_pixel px)
{
    // Compute distance via sdf (This should be some texture lookup later)
    float d = 1.0f - length(pxd.us);
    
    // Create AA factor with the use of ddx/ddy
    float r = lerp(ddx(pxd.us.x), abs(ddy(pxd.us.y)), abs(pxd.us.y));
    
    // Calculate the alpha scaling with use of AA + AA-Scaling 
    float s = smoothstep(0.0f, r * ANT_SDF_aa_scaling, d);
    
    // Write the final color (Should be texture lookup later)
    px.color = float4(pxd.color.rgb * ANT_SFD_textures[pxd.texture_id].Sample(ANT_SDF_image_sampler, pxd.uv).rgb, s);
}

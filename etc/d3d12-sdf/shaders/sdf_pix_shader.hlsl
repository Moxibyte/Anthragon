#include "sdf_pipeline.hlsli"
#include "sdf_bind.hlsli"

[RootSignature(ANT_SDF_ROOTSIG)]
void main(in ant_sdf_vertex pxd, out ant_sdf_pixel px)
{
    float d = 1.0f - length(pxd.uv);
    float r = abs(lerp(ddx(pxd.uv.x), ddy(pxd.uv.y), sin(abs(pxd.uv.y) * 1.57079633f)) );
    float s = smoothstep(0.0f, r * 10.5f, d);
    
    // px.depth = 0.0f;
    px.color = float4(pxd.color.rgb * s, 1.0f);
}
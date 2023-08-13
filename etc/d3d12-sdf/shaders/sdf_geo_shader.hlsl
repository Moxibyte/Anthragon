#include "sdf_pipeline.hlsli"
#include "sdf_bind.hlsli"

[RootSignature(ANT_SDF_ROOTSIG)]
[MaxVertexCount(4)]
void main(in point ant_sdf_quad_pos pt[1], inout TriangleStream<ant_sdf_vertex> ts)
{
    ant_sdf_vertex vtx;
    
    // Index
    uint sdf_idx = pt[0].sdf_desc_idx; 
    
    // UX Cords
    float2 ux_tl = ANT_SDF_sdf_descs_e[sdf_idx].uv_tl;
    float2 ux_br = ANT_SDF_sdf_descs_e[sdf_idx].uv_br;
    
    // Generic vertex properties
    vtx.sdf_desc_idx = sdf_idx;
    vtx.texture_id = pt[0].texture_id;
    vtx.color = pt[0].base_color;

    // Vertex (0)
    vtx.pos = float4(pt[0].pos_tl, 0.0f, 1.0f);
    vtx.uv = pt[0].uv_tl;
    vtx.ux = ux_tl;
    vtx.us = float2(-1.f,  1.f);
    ts.Append(vtx);
    // Vertex(1)
    vtx.pos = float4(pt[0].pos_br.x, pt[0].pos_tl.y, 0.0f, 1.0f);
    vtx.uv = float2(pt[0].uv_br.x, pt[0].uv_tl.y);
    vtx.ux = float2(ux_br.x, ux_tl.y);
    vtx.us = float2( 1.f,  1.f);
    ts.Append(vtx);
    // Vertex(2)
    vtx.pos = float4(pt[0].pos_tl.x, pt[0].pos_br.y, 0.0f, 1.0f);
    vtx.uv = float2(pt[0].uv_tl.x, pt[0].uv_br.y);
    vtx.ux = float2(ux_tl.x, ux_br.y);
    vtx.us = float2(-1.f, -1.f);
    ts.Append(vtx);
    // Vertex(3)
    vtx.pos = float4(pt[0].pos_br, 0.0f, 1.0f);
    vtx.uv = pt[0].uv_br;
    vtx.ux = ux_br;
    vtx.us = float2( 1.f, -1.f);
    ts.Append(vtx);
    
    ts.RestartStrip();
}
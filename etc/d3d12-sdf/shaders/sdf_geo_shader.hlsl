#include "sdf_pipeline.hlsli"
#include "sdf_bind.hlsli"

[RootSignature(ANT_SDF_ROOTSIG)]
[MaxVertexCount(4)]
void main(in point ant_sdf_quad_pos pt[1], inout TriangleStream<ant_sdf_vertex> ts)
{
    ant_sdf_vertex vtx;
    
    // Index
    uint sdf_idx = pt[0].sdf_desc_idx; 
    
    // Rotation matricies
    const float2x2 rotate_p90 = float2x2(0.f, -1.f, 1.f, 0.f);
    const float2x2 rotate_n90 = float2x2(0.f, 1.f, -1.f, 0.f);
    
    // Aspect ratio correction
    const float4 aspect_correction = float4(ANT_SDF_aspect_ratio, 1.f, 1.f, 1.f);
    
    // Access data one (no performence increase or decrese)
    float2 pos = pt[0].pos;
    float4 span = pt[0].span;
    
    // Math for span offsets
    float2 fspan = span.zw - span.xy;
    float fspan_len = length(fspan);
    float fspan_len_h = fspan_len * 0.5f;
    float fspan_tri_b = sqrt(fspan_len * fspan_len_h);
    float fspan_tri_h = sqrt((fspan_tri_b * fspan_tri_b) - (fspan_len_h * fspan_len_h));
    float2 span_off = mad(fspan, 0.5f, span.xy);
    float2 span_vec = normalize(fspan) * fspan_tri_h;
    float2 span_vec_a = (span_off + mul(rotate_n90, span_vec));
    float2 span_vec_b = (span_off + mul(rotate_p90, span_vec));
    
    // Generic vertex properties
    vtx.sdf_desc_idx = sdf_idx;
    vtx.texture_id = pt[0].texture_id;
    vtx.color = pt[0].base_color;

    // Vertex (0)
    vtx.pos = float4(pos + span.xy, 0.0f, 1.0f) * aspect_correction;
    vtx.uv = pt[0].uv_tl;
    vtx.ux = float2(0.f, 0.f);
    vtx.us = float2(-1.f,  1.f);
    ts.Append(vtx);
    // Vertex(1)
    vtx.pos = float4(pos + span_vec_a, 0.0f, 1.0f) * aspect_correction;
    vtx.uv = float2(pt[0].uv_br.x, pt[0].uv_tl.y);
    vtx.ux = float2(1.f, 0.f);
    vtx.us = float2(1.f,  1.f);
    ts.Append(vtx);
    // Vertex(2)
    vtx.pos = float4(pos + span_vec_b, 0.0f, 1.0f) * aspect_correction;
    vtx.uv = float2(pt[0].uv_tl.x, pt[0].uv_br.y);
    vtx.ux = float2(0.f, 1.f);
    vtx.us = float2(-1.f, -1.f);
    ts.Append(vtx);
    // Vertex(3)
    vtx.pos = float4(pos + span.zw, 0.0f, 1.0f) * aspect_correction;
    vtx.uv = pt[0].uv_br;
    vtx.ux = float2(1.f, 1.f);
    vtx.us = float2( 1.f, -1.f);
    ts.Append(vtx);
    
    ts.RestartStrip();
}
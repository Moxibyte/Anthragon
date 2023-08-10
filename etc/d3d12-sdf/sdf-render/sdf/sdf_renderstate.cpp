#include "sdf_renderstate.h"

ant::sdf::sdf_renderstate::sdf_renderstate(d3d_context::ptr ctx) :
    m_ctx(ctx)
{
    create_pso();
    create_desc_heaps();
    create_samplers();
}

ant::sdf::sdf_renderstate::texture_slot ant::sdf::sdf_renderstate::allocate_texture(ID3D12Resource* texture, D3D12_SHADER_RESOURCE_VIEW_DESC& texture_srv)
{
    if (!m_texture_free_list.empty())
    {
        texture_slot slot = m_texture_free_list.front();
        m_texture_free_list.pop();
        m_ctx->get_device()->CreateShaderResourceView(texture, &texture_srv, m_desc_heap_textures->get_cpu(slot));
        return slot;
    }
    return -1;
}

void ant::sdf::sdf_renderstate::free_texture(texture_slot slot)
{
    m_texture_free_list.push(slot);
}

void ant::sdf::sdf_renderstate::bind(d3d_command_list::ptr cmd_list, d3d_resource::ptr sdf_descriptors)
{
    cmd_list->ia_set_primitive_topology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
    cmd_list->set_pso_rs_gfx(m_pipeline_state, m_root_signature);
    cmd_list->set_descriptor_heaps(m_desc_heap_textures->get_ptr(), m_desc_heap_samplers->get_ptr());
    cmd_list->set_graphics_root_descriptor_table(0, m_desc_heap_textures->get_gpu(0));
    cmd_list->set_graphics_root_constant_buffer_view(1, sdf_descriptors->gpu_address());
    cmd_list->set_graphics_root_descriptor_table(2, m_desc_heap_samplers->get_gpu(0));
}

void ant::sdf::sdf_renderstate::create_pso()
{
    // Files
    blob shader_data_rs(get_binary_directory() / "shaders" / "sdf_pipeline.cso");
    blob shader_data_vs(get_binary_directory() / "shaders" / "sdf_vtx_shader.cso");
    blob shader_data_gs(get_binary_directory() / "shaders" / "sdf_geo_shader.cso");
    blob shader_data_ps(get_binary_directory() / "shaders" / "sdf_pix_shader.cso");

    // Root Signature
    ANT_CHECK_HR(
        m_ctx->get_device()->CreateRootSignature(0, shader_data_rs.data(), shader_data_rs.size(), IID_PPV_ARGS(&m_root_signature)), 
        "Failed to create root signature"
    );

    // Input layout
    D3D12_INPUT_ELEMENT_DESC input_layout[] =
    {
        { "TopLeft", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BottomRight", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TopLeftUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BottomRightUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "SDFDesc", 0, DXGI_FORMAT_R32_UINT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "Texture", 0, DXGI_FORMAT_R32_UINT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // Pipeline State
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psd{};
    psd.pRootSignature = m_root_signature;
    psd.VS.pShaderBytecode = shader_data_vs.data();
    psd.VS.BytecodeLength = shader_data_vs.size();
    psd.GS.pShaderBytecode = shader_data_gs.data();
    psd.GS.BytecodeLength = shader_data_gs.size();
    psd.PS.pShaderBytecode = shader_data_ps.data();
    psd.PS.BytecodeLength = shader_data_ps.size();
    psd.StreamOutput.NumEntries = 0;
    psd.StreamOutput.NumStrides = 0;
    psd.StreamOutput.pBufferStrides = nullptr;
    psd.StreamOutput.pSODeclaration = nullptr;
    psd.StreamOutput.RasterizedStream = 0;
    psd.BlendState.AlphaToCoverageEnable = false;
    psd.BlendState.IndependentBlendEnable = false;
    psd.BlendState.RenderTarget[0].BlendEnable = false;
    psd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
    psd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
    psd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psd.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    psd.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    psd.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    psd.BlendState.RenderTarget[0].LogicOpEnable = false;
    psd.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    psd.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    psd.SampleMask = 0xFFFFFFFF;
    psd.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    psd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psd.RasterizerState.FrontCounterClockwise = false;
    psd.RasterizerState.DepthBias = 0;
    psd.RasterizerState.DepthBiasClamp = .0f;
    psd.RasterizerState.SlopeScaledDepthBias = .0f;
    psd.RasterizerState.DepthClipEnable = false;
    psd.RasterizerState.MultisampleEnable = false;
    psd.RasterizerState.AntialiasedLineEnable = false;
    psd.RasterizerState.ForcedSampleCount = 0;
    psd.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    psd.DepthStencilState.DepthEnable = false;
    psd.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    psd.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    psd.DepthStencilState.StencilEnable = false;
    psd.DepthStencilState.StencilReadMask = 0;
    psd.DepthStencilState.StencilWriteMask = 0;
    psd.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    psd.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    psd.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    psd.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    psd.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    psd.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    psd.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    psd.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    psd.InputLayout.NumElements = _countof(input_layout);
    psd.InputLayout.pInputElementDescs = input_layout;
    psd.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    psd.NumRenderTargets = 1;
    psd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psd.DSVFormat = DXGI_FORMAT_UNKNOWN;
    psd.SampleDesc.Count = 1;
    psd.SampleDesc.Quality = 0;
    psd.NodeMask = 0;
    psd.CachedPSO.CachedBlobSizeInBytes = 0;
    psd.CachedPSO.pCachedBlob = nullptr;
    psd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    ANT_CHECK_HR(m_ctx->get_device()->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(&m_pipeline_state)), "Failed to create pipeline state");
}

void ant::sdf::sdf_renderstate::create_desc_heaps()
{
    m_desc_heap_textures = std::make_shared<d3d_descriptor_heap>(m_ctx, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, c_max_texture);
    for (size_t i = 0; i < c_max_texture; ++i)
    {
        m_texture_free_list.push(i);
    }

    m_desc_heap_samplers = std::make_shared<d3d_descriptor_heap>(m_ctx, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 2);
}

void ant::sdf::sdf_renderstate::create_samplers()
{
    D3D12_SAMPLER_DESC texture_sampler{};
    texture_sampler.Filter = D3D12_FILTER_ANISOTROPIC;
    texture_sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    texture_sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    texture_sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    texture_sampler.MipLODBias = 0;
    texture_sampler.MaxAnisotropy = 16;
    texture_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    texture_sampler.MinLOD = 0;
    texture_sampler.MaxLOD = 0;
    m_ctx->get_device()->CreateSampler(&texture_sampler, m_desc_heap_samplers->get_cpu(0));
    
    D3D12_SAMPLER_DESC sdf_sampler{};
    sdf_sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sdf_sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sdf_sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sdf_sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sdf_sampler.MipLODBias = 0;
    sdf_sampler.MaxAnisotropy = 0;
    sdf_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    sdf_sampler.MinLOD = 0;
    sdf_sampler.MaxLOD = 0;
    m_ctx->get_device()->CreateSampler(&sdf_sampler, m_desc_heap_samplers->get_cpu(1));
}

#pragma once

#include <sdf-render/util/dir.h>
#include <sdf-render/util/blob.h>
#include <sdf-render/sdf/sdf_layout.h>
#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_uploader.h>
#include <sdf-render/d3d/d3d_resource.h>
#include <sdf-render/d3d/d3d_command_list.h>
#include <sdf-render/d3d/d3d_descriptor_heap.h>

#include <queue>
#include <cstdlib>

namespace ant::sdf
{
    class sdf_renderstate
    {
        public:
            using ptr = std::shared_ptr<sdf_renderstate>;
            using texture_slot = size_t;

        public:
            sdf_renderstate() = delete;
            sdf_renderstate(d3d_context::ptr ctx);
            sdf_renderstate(const sdf_renderstate&) = delete;
            sdf_renderstate(sdf_renderstate&&) noexcept = delete;

            sdf_renderstate& operator=(const sdf_renderstate&) = delete;
            sdf_renderstate& operator=(sdf_renderstate&&) noexcept = delete;

            texture_slot allocate_texture(ID3D12Resource* texture, D3D12_SHADER_RESOURCE_VIEW_DESC& texture_srv);
            void free_texture(texture_slot slot);

            void bind(d3d_command_list::ptr cmd_list, d3d_resource::ptr sdf_descriptors);
            
        private:
            void create_pso();
            void create_desc_heaps();
            void create_samplers();

        private:
            const size_t c_max_texture = 128;

            d3d_context::ptr m_ctx;

            comptr<ID3D12RootSignature> m_root_signature;
            comptr<ID3D12PipelineState> m_pipeline_state;

            d3d_descriptor_heap::ptr m_desc_heap_textures;
            std::queue<texture_slot> m_texture_free_list;

            d3d_descriptor_heap::ptr m_desc_heap_samplers;
    };
}

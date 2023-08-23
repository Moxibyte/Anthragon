#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_executor.h>
#include <sdf-render/d3d/d3d_resource.h>
#include <sdf-render/d3d/d3d_uploader.h>
#include <sdf-render/d3d/d3d_command_list.h>
#include <sdf-render/d3d/d3d_texture_2d_rgba.h>
#include <sdf-render/presentation/window.h>

#include <sdf-render/sdf/sdf_renderer.h>

#include <iostream>
#include <chrono>

const float COLOR_RED[] = { 1.0f, 0.0f, 0.0f, 1.0f };
const float COLOR_GREEN[] = { 0.0f, 1.0f, 0.0f, 1.0f };
const float COLOR_BLUE[] = { 0.0f, 0.0f, 1.0f, 1.0f };
const float COLOR_WHITE[] = { 1.0f, 1.0f, 1.0f, 1.0f };

const float ANGEL_VELOCITY = 0.0005f;

void compute_positon(float pos[2], float a, float r, float w[2])
{
    const float radius = 0.7f;
    pos[0] = radius * cos(a) - w[0] / 2;
    pos[1] = radius * sin(a) + w[1] / 2;
}

int d3d_main(ant::sdf::d3d_context::ptr ctx)
{
    using namespace ant::sdf;
    d3d_executor::ptr executor = std::make_shared<d3d_executor>(ctx);
    d3d_uploader::ptr uploader = std::make_shared<d3d_uploader>(ctx, 16 * 1024 * 1024);
    d3d_command_list::ptr list = std::make_shared<d3d_command_list>(ctx);
    
    window::ptr wnd = std::make_shared<window>(
        ctx, executor, L"SDF Sample Renderer", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        WS_EX_OVERLAPPEDWINDOW, 
        10, 10, 500, 500
    );

    // SDF Preprocessing
    sdf_renderer::ptr state = std::make_shared<sdf_renderer>(ctx);
    state->process_sdfs(128);
    state->upload_sdf_data(executor, uploader, list);

    // Images
    d3d_texture_2d_rgba::ptr texture1 = std::make_shared<d3d_texture_2d_rgba>(ctx, executor, uploader, list, "resources/pix_e1.png");
    d3d_texture_2d_rgba::ptr texture2 = std::make_shared<d3d_texture_2d_rgba>(ctx, executor, uploader, list, "resources/pix_e2.png");
    d3d_texture_2d_rgba::ptr texture3 = std::make_shared<d3d_texture_2d_rgba>(ctx, executor, uploader, list, "resources/pix_e3.png");
    d3d_texture_2d_rgba::ptr texture4 = std::make_shared<d3d_texture_2d_rgba>(ctx, executor, uploader, list, "resources/pix_e4.png");

    // SRVs
    D3D12_SHADER_RESOURCE_VIEW_DESC srv1, srv2, srv3, srv4;
    texture1->create_srv(srv1);
    texture2->create_srv(srv2);
    texture3->create_srv(srv3);
    texture4->create_srv(srv4);
    auto ref1 = state->allocate_texture(texture1->get_ptr(), srv1);
    auto ref2 = state->allocate_texture(texture2->get_ptr(), srv2);
    auto ref3 = state->allocate_texture(texture3->get_ptr(), srv3);
    auto ref4 = state->allocate_texture(texture4->get_ptr(), srv4);

    // SDFs
    auto sdfd1 = state->allocate_sdf_desc(sdf_renderer::SDFID_line, 0.05f);
    auto sdfd2 = state->allocate_sdf_desc(sdf_renderer::SDFID_ring, 0.1f);
    auto sdfd3 = state->allocate_sdf_desc(sdf_renderer::SDFID_circle);
    auto sdfd4 = state->allocate_sdf_desc(sdf_renderer::SDFID_triangle);

    // UI Loop
    std::chrono::high_resolution_clock::time_point now, last;
    now = std::chrono::high_resolution_clock::now();
    while (!wnd->should_close())
    {
        // Window processing
        wnd->process_messages();
        if (wnd->should_resize())
        {
            executor->flush(2);
            wnd->resize();
        }

        // Timing
        last = now;
        now = std::chrono::high_resolution_clock::now();
        auto d = now - last;
        auto dx = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();

        // Animate angel
        static float angel = 0.00f;
        angel += ANGEL_VELOCITY * dx;
        if (angel > 6.2831853f) angel -= 6.2831853f;

        // Build position from angel
        float positions[8];
        float ratio = (float)wnd->get_height() / (float)wnd->get_width();
        float size[] = {0.5f, 0.5f};
        compute_positon(&positions[0], angel + 0.000000f, ratio, size);
        compute_positon(&positions[2], angel + 1.570796f, ratio, size);
        compute_positon(&positions[4], angel + 3.141593f, ratio, size);
        compute_positon(&positions[6], angel + 4.712389f, ratio, size);
        
        // SDF config
        state->set_aspect_ratio(ratio);

        // Stage rendering
        state->quad_atl(&positions[0], size, angel + 4.712389f, COLOR_WHITE, ref1, sdfd1);
        state->quad_atl(&positions[2], size, angel + 3.141593f, COLOR_WHITE, ref2, sdfd2);
        state->quad_atl(&positions[4], size, angel + 1.570796f, COLOR_WHITE, ref3, sdfd3);
        state->quad_atl(&positions[6], size, angel + 0.000000f, COLOR_WHITE, ref4, sdfd4);

        // Begin window rendering
        wnd->begin_frame(list);

        // GPU Rendering
        state->copy_data(list, uploader);
        state->bind(list);
        state->draw(list);

        // End frame, execute and reset
        wnd->end_frame(list);
        list->execute_sync(executor);
        list->reset();
        uploader->reset();

        // Preset & Pull debug layer
        wnd->present();
        ctx->pull_messages();
    }
    // Assert cleanup
    executor->flush(2);

    return 0;
}

int main()
{
    using namespace ant::sdf;
    std::shared_ptr<d3d_context> ctx;
    int rc = 0;
    
    try
    {
        ctx = std::make_shared<d3d_context>();
        rc = d3d_main(ctx);
    }
    catch (const std::exception& ex)
    {
        std::cout << "Exception occurred:" << std::endl;
        std::cout << ex.what() << std::endl;
        rc = -2;

        MessageBoxA(nullptr, ex.what(), "Exception occurred", MB_OK | MB_ICONEXCLAMATION);
    }

    // Final msg pull
    if (ctx)
    {
        try
        {
            ctx->pull_messages();
        }
        catch(...)
        {
            rc--;
        }
    }

    // 0   -> OK
    // -1  -> Last pull failed
    // -2  -> Exception in main
    // -3  -> Exception in main and last pull failed
    return rc;
}

#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_executor.h>
#include <sdf-render/d3d/d3d_resource.h>
#include <sdf-render/d3d/d3d_uploader.h>
#include <sdf-render/d3d/d3d_command_list.h>
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
    pos[0] = radius * cos(a) * r - w[0] / 2;
    pos[1] = radius * sin(a) + w[1] / 2;
}

int d3d_main(ant::sdf::d3d_context::ptr ctx)
{
    using namespace ant::sdf;
    d3d_executor::ptr executor = std::make_shared<d3d_executor>(ctx);
    d3d_uploader::ptr uploader = std::make_shared<d3d_uploader>(ctx, 1024 * 64);
    d3d_command_list::ptr list = std::make_shared<d3d_command_list>(ctx);
    
    window::ptr wnd = std::make_shared<window>(
        ctx, executor, L"Demo 1234", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        WS_EX_OVERLAPPEDWINDOW, 
        10, 10, 500, 500
    );
    sdf_renderer::ptr state = std::make_shared<sdf_renderer>(ctx);
    
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
        float size[] = {0.5f * ratio, 0.5f};
        compute_positon(&positions[0], angel + 0.000000f, ratio, size);
        compute_positon(&positions[2], angel + 1.570796f, ratio, size);
        compute_positon(&positions[4], angel + 3.141593f, ratio, size);
        compute_positon(&positions[6], angel + 4.712389f, ratio, size);

        // Stage rendering
        state->stage_colored_quad(&positions[0], size, COLOR_RED);
        state->stage_colored_quad(&positions[2], size, COLOR_GREEN);
        state->stage_colored_quad(&positions[4], size, COLOR_BLUE);
        state->stage_colored_quad(&positions[6], size, COLOR_WHITE);

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

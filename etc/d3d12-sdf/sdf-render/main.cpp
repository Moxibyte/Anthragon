#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_executor.h>
#include <sdf-render/d3d/d3d_resource.h>
#include <sdf-render/d3d/d3d_command_list.h>
#include <sdf-render/presentation/window.h>

#include <sdf-render/sdf/sdf_renderstate.h>

#include <iostream>

int d3d_main(ant::sdf::d3d_context::ptr ctx)
{
    using namespace ant::sdf;
    d3d_executor::ptr executor = std::make_shared<d3d_executor>(ctx);
    d3d_command_list::ptr list = std::make_shared<d3d_command_list>(ctx);
    
    sdf_renderstate::ptr state = std::make_shared<sdf_renderstate>(ctx);

    window::ptr wnd = std::make_shared<window>(ctx, executor, L"Demo 1234", WS_OVERLAPPEDWINDOW | WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW, 10, 10, 500, 500);
    
    while (!wnd->should_close())
    {
        // Window processing
        wnd->process_messages();
        if (wnd->should_resize())
        {
            executor->flush(2);
            wnd->resize();
        }

        // Begin window rendering
        wnd->begin_frame(list);

        // TODO: Rendering

        // End frame, execute and reset
        wnd->end_frame(list);
        list->execute_sync(executor);
        list->reset();

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
        std::cout << "Exception occured:" << std::endl;
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

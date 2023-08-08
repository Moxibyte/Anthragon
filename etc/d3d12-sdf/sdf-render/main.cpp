#include <sdf-render/d3d/context.h>

#include <iostream>

int d3d_main(std::shared_ptr<ant::sdf::d3d_context>& ctx)
{
    using namespace ant::sdf;

    
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

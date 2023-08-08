#include <sdf-render/d3d/context.h>

#include <iostream>

int main()
{
    using namespace ant::sdf;
    try
    {
        d3d_context ctx;
        std::cout << "HelloWorld!" << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

cmake_minimum_required (VERSION 3.12)

# Configure build
option(ANT_BUILD_EXAMPLES "Build the example code" OFF)


# Main source directory
add_subdirectory(src)

# Example code
if(ANT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif(ANT_BUILD_EXAMPLES)

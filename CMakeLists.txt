cmake_minimum_required(VERSION 3.23)
project(3d_Renderer)

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(FATAL_ERROR "This project is g++ only (for now maybe)")
endif()

add_compile_definitions(_GLIBCXX_DEBUG=1)

add_compile_options(-g -Wall -Wextra -Wpedantic)
add_compile_options(-fsanitize=undefined,address,leak)
add_link_options(-fsanitize=undefined,address,leak)

include_directories(eigen)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main src/main.cpp src/renderer.cpp src/rasterizer.cpp src/screen.cpp src/world.cpp src/primitive.cpp src/object.cpp src/geometry.cpp src/geometry/algo.cpp src/geometry/plane.cpp src/geometry/sphere.cpp src/geometry/line.cpp)

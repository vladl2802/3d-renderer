cmake_minimum_required(VERSION 3.23)
project(3d_Renderer)

add_compile_options(-g -Wall -Wextra -Wpedantic)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

include_directories(/usr/local/include/eigen3)

set(CMAKE_CXX_STANDARD 20)

add_executable(main src/main.cpp src/renderer.cpp src/world.cpp src/primitive.cpp src/object.cpp src/geometry.cpp src/geometry/algo.cpp src/geometry/plane.cpp src/geometry/sphere.cpp)

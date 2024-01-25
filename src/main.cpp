#include <array>
#include <vector>
#include <iostream>

#include "geometry.h"
#include "renderer.h"

using renderer::Object;
using renderer::Renderer;
using renderer::World;
using renderer::types::Matrix;

int main() {
    std::vector<int> a;
    a.push_back(10);

    auto t1 = Object::create_triangle({2, 5, 0}, {-3, 6, 1}, {3, 8, 2}, {0.5, 0.5, 0.5});
    std::cerr << "Triangle is created\n";
    World world;
    world.push_object(t1);
    std::cerr << "World is created\n";
    Matrix<3, 3> orientation{{1, 0, 0}, {0, 0, -1}, {0, 1, 0}};
    renderer::Camera camera = {.position = {0, 0, 0},
                               .left = -1,
                               .right = 1,
                               .bottom = -1,
                               .top = 1,
                               .orientation = orientation,
                               .near = 1,
                               .far = 11};
    auto renderer = Renderer(20, 20);
    std::cerr << "Renderer is created\n";
    auto screen = renderer(world, camera).get_frame_buffer();
    for (const auto& row : screen) {
        for (const auto& elem : row) {
            if (elem.x() > 0) std::cout << "x";
            else std::cout << ".";
        }
        std::cout << "\n";
    }
}

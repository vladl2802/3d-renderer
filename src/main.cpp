#include <array>

#include "geometry.h"
#include "renderer.h"

int main() {
    // renderer::types::Matrix<4, 4> operation;
    // using Point = renderer::primitive::Point;
    // using GeomPoint = renderer::types::Point;
    // using RGBColor = renderer::types::RGBColor;
    // auto temp = [](const int& primitive) {
    // };
    // // auto t = Point({GeomPoint(0, 0, 0)}, RGBColor(0, 0, 0));
    // auto t = 0;
    // temp(t);
    // static_assert(std::is_invocable_v<decltype(temp), const decltype(t)>);
    renderer::Renderer(100, 100);
}

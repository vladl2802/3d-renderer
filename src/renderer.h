#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#include "geometry.h"
#include "geometry/plane.h"
#include "primitive.h"
#include "world.h"

namespace renderer {
struct Camera {
    Point position;
    float width;
    float height;
    Matrix<3, 3> direction;
    float near;
    float far;
};

template <unsigned int Height, unsigned int Width>
using Screen = std::array<std::array<RGBColor, Width>, Height>;

template <unsigned int Height, unsigned int Width>
class Renderer {
    Renderer();

    const Screen<Height, Width>& operator()(const World& world, const Camera& camera);

private:
    static Matrix<4, 4> get_to_camera_coords_matrix(const Camera& camera);
    static std::array<Plane, 6> get_view_frustum_bounding(const Camera& camera);
    static Matrix<4, 4> get_perspective_projection_matrix();

    Screen<Height, Width> screen;

    // std::vector<Primitive> visible_primitives;
};

template <unsigned int Height, unsigned int Width>
Renderer<Height, Width>::Renderer() {
}

template <unsigned int Height, unsigned int Width>
inline Matrix<4, 4> Renderer<Height, Width>::get_to_camera_coords_matrix(const Camera& camera) {
    Matrix<4, 4> result = Matrix<4, 4>::Zero();
    result.block<3, 3>(0, 0) = camera.direction;
    result.block<3, 1>(0, 3) = -camera.position;
    result(3, 3) = 1;
    return result;
}

template <unsigned int Height, unsigned int Width>
inline std::array<Plane, 6> Renderer<Height, Width>::get_view_frustum_bounding(
    const Camera& camera) {
    std::array<Plane, 6> result = {
        Plane(Vector3(0, 0, -1), Point(0, 0, -near)),                         // near
        Plane(Vector3(0, 0, 1), Point(0, 0, -far)),                           // far
        Plane(Vector3(camera.near, 0, -camera.width / 2), Point(0, 0, 0)),    // left
        Plane(Vector3(-camera.near, 0, -camera.width / 2), Point(0, 0, 0)),   // right
        Plane(Vector3(0, camera.near, -camera.height / 2), Point(0, 0, 0)),   // bottom
        Plane(Vector3(0, -camera.near, -camera.height / 2), Point(0, 0, 0)),  // top
    };
    auto transform_matrix = get_to_camera_coords_matrix(camera).inverse();
    for (auto& plane : result) {
        plane = plane.transform(transform_matrix);
    }
    return result;
}

template <unsigned int Height, unsigned int Width>
const Screen<Height, Width>& Renderer<Height, Width>::operator()(const World& world,
                                                                 const Camera& camera) {
    assert(std::abs(Camera.height * width - Camera.width * height) < 1e-4 &
           "Aspect ratios of camera and screen must be equal");

    // Need to check camera direction matrix (that it's contains normalized right vector triple)\

    auto transform_matrix = get_to_camera_coords_matrix(camera);
    auto frustum_bounding = get_view_frustum_bounding(camera);
    for (const auto& obj : world.objects_) {
        std::array<BoundingCheckResult, 6> check_results;
        std::transform(frustum_bounding.begin(), frustum_bounding.end(), check_results.begin(),
                       [&obj](auto plane) { return obj.check_bounding(plane); });

        if (std::any_of(check_results.begin(), check_results.end(),
                        [](auto res) { return res == BoundingCheckResult::OnNegativeSide })) {
            // If object is outside view frustum it can be fully skipped
            continue;
        }
        if (std::any_of(check_results.begin(), check_results.end(),
                        [](auto res) { return res == BoundingCheckResult::Intersects })) {
            // If object intersects view frustum it needs further checks on each its primitive
        } else {
            // If object is inside view frustum it can be fully drawn
        }
    }

    return screen;
}
}  // namespace renderer
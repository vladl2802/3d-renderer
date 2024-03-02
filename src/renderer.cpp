#include "renderer.h"

#include <memory>

#include "Eigen/LU"
#include "rasterizer.h"

namespace renderer {

Renderer::Matrix<4, 4> Renderer::get_from_camera_coords_matrix(const Camera& camera) {
    Matrix<4, 4> result = Matrix<4, 4>::Zero();
    result.block<3, 3>(0, 0) = camera.orientation;
    result.block<3, 1>(0, 3) = -camera.position;
    result(3, 3) = 1;
    return result;
}

std::array<Plane, 6> Renderer::get_view_frustum_bounding(const Camera& camera) {
    std::array<Plane, 6> result = {
        Plane(Vector3(0, 0, -1), Point(0, 0, -camera.near)),             // near
        Plane(Vector3(0, 0, 1), Point(0, 0, -camera.far)),               // far
        Plane(Vector3(camera.near, 0, camera.left), Point(0, 0, 0)),     // left
        Plane(Vector3(-camera.near, 0, -camera.right), Point(0, 0, 0)),  // right
        Plane(Vector3(0, camera.near, camera.bottom), Point(0, 0, 0)),   // bottom
        Plane(Vector3(0, -camera.near, -camera.top), Point(0, 0, 0)),    // top
    };
    return result;
}

std::array<Plane, 6> Renderer::transform_bounding(const std::array<Plane, 6>& bounding,
                                                  const Matrix<4, 4>& operation) {
    std::array<Plane, 6> result;
    std::transform(bounding.begin(), bounding.end(), result.begin(),
                   [&operation](const Plane& plane) { return plane.transform(operation); });
    return result;
}

Renderer::Matrix<4, 4> Renderer::get_perspective_projection_matrix(const Camera& camera) {
    auto n = camera.near, f = camera.far;
    auto l = camera.left, r = camera.right;
    auto b = camera.bottom, t = camera.top;
    Matrix<4, 4> result{{2 * n / (r - l), 0, (r + l) / (r - l), 0},
                        {0, 2 * n / (t - b), (t + b) / (t - b), 0},
                        {0, 0, -(f + n) / (f - n), -2 * n * f / (f - n)},
                        {0, 0, -1, 0}};
    return result;
}

std::array<BoundingCheckResult, 6> Renderer::check_bounding(const std::array<Plane, 6>& bounding,
                                                            const Object& object) {
    std::array<BoundingCheckResult, 6> result;
    std::transform(bounding.begin(), bounding.end(), result.begin(),
                   [&object](const Plane& plane) { return object.check_bounding(plane); });
    return result;
}

bool Renderer::can_skip_object(const std::array<BoundingCheckResult, 6>& check_results) {
    return std::any_of(check_results.begin(), check_results.end(),
                       [](auto res) { return res == BoundingCheckResult::OnNegativeSide; });
}

bool Renderer::need_intersect_object(const std::array<BoundingCheckResult, 6>& check_results) {
    return std::any_of(check_results.begin(), check_results.end(),
                       [](auto res) { return res == BoundingCheckResult::Intersects; });
}

Renderer::Renderer(size_t screen_height, size_t screen_width)
    : dims_({.height = screen_height, .width = screen_width}) {
}

Screen Renderer::operator()(const World& world, const Camera& camera) {
    // Need to check camera direction matrix (that it contains normalized right vector triple)

    // I want to give rasterizer temporal (for it's lifetime) ownership over screen and get it back,
    // but I don't know to do it properly
    auto rasterizer = Rasterizer(dims_);
    Matrix<4, 4> transform_from_camera = get_from_camera_coords_matrix(camera);
    std::array<Plane, 6> frustum_bounding = get_view_frustum_bounding(camera);
    std::array<Plane, 6> transformed_frustum_bounding =
        transform_bounding(frustum_bounding, transform_from_camera);
    Matrix<4, 4> transform_to_camera =
        get_perspective_projection_matrix(camera) * transform_from_camera.inverse();
    for (const auto& obj : world.get_objects()) {
        auto check_results = check_bounding(transformed_frustum_bounding, obj);
        if (can_skip_object(check_results)) {
            // If object is outside view frustum it can be fully skipped
            continue;
        }
        PrimitivesSet primitives = obj.get_primitives();
        if (need_intersect_object(check_results)) {
            primitives = primitives.intersect(transformed_frustum_bounding);
        }
        primitives.transform_inplace(transform_to_camera);
        primitives.for_each([&rasterizer](auto prim) { rasterizer(prim); });
    }
    return rasterizer.get_screen();
}

}  // namespace renderer

#include "renderer.h"

namespace renderer {

types::Matrix<4, 4> Renderer::get_to_camera_coords_matrix(const Camera& camera) {
    Matrix<4, 4> result = Matrix<4, 4>::Zero();
    result.block<3, 3>(0, 0) = camera.orientation;
    result.block<3, 1>(0, 3) = -camera.position;
    result(3, 3) = 1;
    return result;
}

std::array<Plane, 6> Renderer::get_view_frustum_bounding(const Camera& camera) {
    std::array<Plane, 6> result = {
        Plane(Vector3(0, 0, -1), Point(0, 0, -camera.near)),                  // near
        Plane(Vector3(0, 0, 1), Point(0, 0, -camera.far)),                    // far
        Plane(Vector3(camera.near, 0, -camera.width / 2), Point(0, 0, 0)),    // left
        Plane(Vector3(-camera.near, 0, -camera.width / 2), Point(0, 0, 0)),   // right
        Plane(Vector3(0, camera.near, -camera.height / 2), Point(0, 0, 0)),   // bottom
        Plane(Vector3(0, -camera.near, -camera.height / 2), Point(0, 0, 0)),  // top
    };
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

bool Renderer::can_draw_object(const std::array<BoundingCheckResult, 6>& check_results) {
    return std::all_of(check_results.begin(), check_results.end(),
                       [](auto res) { return res == BoundingCheckResult::OnPositiveSide; });
}

Renderer::Renderer(size_t screen_height, size_t screen_width)
    : screen_(screen_height, screen_width) {
}

const Screen& Renderer::operator()(const World& world, const Camera& camera) {
    // Need to check camera direction matrix (that it contains normalized right vector triple)

    auto transform_matrix = get_to_camera_coords_matrix(camera);
    
    std::array<Plane, 6> frustum_bounding = get_view_frustum_bounding(camera);
    std::array<Plane, 6> transformed_frustum_bounding;
    std::transform(frustum_bounding.begin(), frustum_bounding.end(),
                   transformed_frustum_bounding.begin(),
                   [transform_matrix = transform_matrix.inverse()](const Plane& plane) {
                       return plane.transform(transform_matrix);
                   });
    for (const auto& obj : world.get_objects()) {
        auto check_results = check_bounding(transformed_frustum_bounding, obj);
        if (can_skip_object(check_results)) {
            // If object is outside view frustum it can be fully skipped
            continue;
        }
        PrimitivesSet primitives = obj.get_primitives();
        primitives.transform_inplace(transform_matrix);
        if (can_draw_object(check_results)) {
            // If object is inside view frustum it can be fully drawn
            primitives.rasterize(screen_);
        } else {
            // If object intersects view frustum it needs further checks on each its primitive
            primitives.intersect_and_rasterize(frustum_bounding, screen_);
        }
    }
    return screen_;
}

}  // namespace renderer

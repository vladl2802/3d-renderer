#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "geometry.h"
#include "geometry/plane.h"
#include "primitive.h"
#include "screen.h"
#include "world.h"

namespace renderer {

struct Camera {
    using CordType = types::CordType;
    using Point = types::Point;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;

    Point position;
    CordType left;
    CordType right;
    CordType bottom;
    CordType top;
    Matrix<3, 3> orientation;
    CordType near;
    CordType far;
};

class Renderer {
public:
    using Point = types::Point;
    using Vector3 = types::Vector3;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;

    Renderer(size_t screen_height, size_t screen_width);

    const Screen& operator()(const World& world, const Camera& camera);

private:
    static Matrix<4, 4> get_from_camera_coords_matrix(const Camera& camera);
    static std::array<Plane, 6> get_view_frustum_bounding(const Camera& camera);
    static std::array<Plane, 6> transform_bounding(const std::array<Plane, 6>& bounding,
                                                   const Matrix<4, 4>& operation);
    static Matrix<4, 4> get_perspective_projection_matrix(const Camera& camera);

    static std::array<BoundingCheckResult, 6> check_bounding(const std::array<Plane, 6>& bounding,
                                                             const Object& object);
    static bool can_skip_object(const std::array<BoundingCheckResult, 6>& check_results);
    static bool can_draw_object(const std::array<BoundingCheckResult, 6>& check_results);

    Screen screen_;
};

}  // namespace renderer

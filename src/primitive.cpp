#include "primitive.h"

#include <algorithm>
#include <memory>
#include <numeric>
#include <tuple>

#include "eigen3/Eigen/Geometry"
#include "geometry/algo.h"
#include "geometry/linear_interpolation.h"

namespace renderer {

namespace {

using types::Matrix;
using types::Point;
using types::Vector4;

inline void homogeneous_transform_inplace(Point& position, const Matrix<4, 4>& operation) {
    Vector4 new_pos = operation * position.homogeneous();
    position = new_pos.head<3>() / new_pos(3);
}

template <size_t N>
inline void perform_transform_inplace(std::array<Point, N>& vertices,
                                      const Matrix<4, 4>& operation) {
    for (size_t i = 0; i < N; ++i) {
        homogeneous_transform_inplace(vertices[i], operation);
    }
}

}  // namespace

namespace primitive {

Point::Point(GeomPoint position, RGBColor color) : color_(color), position_(position) {
}

void Point::transform_inplace(const Matrix<4, 4>& operation) {
    homogeneous_transform_inplace(position_, operation);
}

void Point::rasterize(Screen& screen) const {
    screen.put_pixel_in_camera_cords(position_, color_);
}

std::vector<Point> Point::intersect(const Plane& plane) const {
    if (plane.get_signed_distance(position_) > 0) {
        return {*this};
    } else {
        return {};
    }
}

std::vector<Point::GeomPoint> Point::get_vertices() const {
    return {position_};
}

Segment::Segment(std::array<GeomPoint, 2> vertices, RGBColor color)
    : color_(color), vertices_(vertices) {
}

void Segment::transform_inplace(const Matrix<4, 4>& operation) {
    perform_transform_inplace<2>(vertices_, operation);
}

void Segment::rasterize(Screen& screen) const {
    // This is really inefficient, so needs to be rewritten
}

std::vector<Segment> Segment::intersect(const Plane& plane) const {
    auto check_is_outside = [&plane](const GeomPoint& p) {
        return plane.get_signed_distance(p) < 0;
    };
    size_t outside_count = std::count_if(vertices_.begin(), vertices_.end(), check_is_outside);
    if (outside_count == 2) {
        return {};
    }
    if (outside_count == 0) {
        return {*this};
    }
    Segment result = *this;
    // This may be inconsistent so std::array<bool, 2> of check results may be preferable
    auto outside_point =
        std::find_if(result.vertices_.begin(), result.vertices_.end(), check_is_outside);
    assert(outside_point != vertices_.end());
    *outside_point = Line(vertices_[0], vertices_[1]).intersect_plane(plane);
    return {result};
}

std::vector<Segment::GeomPoint> Segment::get_vertices() const {
    return std::vector<Segment::GeomPoint>(vertices_.begin(), vertices_.end());
}

Triangle::Triangle(std::array<GeomPoint, 3> vertices, RGBColor color)
    : color_(color), vertices_(vertices) {
}

void Triangle::transform_inplace(const Matrix<4, 4>& operation) {
    return perform_transform_inplace<3>(vertices_, operation);
}

void Triangle::rasterize(Screen& screen) const {
    // This is not efficient for thin triangles, so maybe add another implementation for such
    auto dims = screen.get_dimensions();
    // move this to separate function
    std::array<CordType, 3> xs_tmp = {vertices_[0].x(), vertices_[1].x(), vertices_[2].x()};
    std::array<CordType, 3> ys_tmp = {vertices_[0].y(), vertices_[1].y(), vertices_[2].y()};
    size_t x_min = (*std::min_element(xs_tmp.begin(), xs_tmp.end()) + 1) * dims.width / 2;
    size_t x_max = (*std::max_element(xs_tmp.begin(), xs_tmp.end()) + 1) * dims.width / 2 + 1;
    size_t y_min = (*std::min_element(ys_tmp.begin(), ys_tmp.end()) + 1) * dims.height / 2;
    size_t y_max = (*std::max_element(ys_tmp.begin(), ys_tmp.end()) + 1) * dims.height / 2 + 1;
    for (size_t x_ind = std::max(x_min, 0ul); x_ind < std::min(x_max, dims.width); ++x_ind) {
        for (size_t y_ind = std::max(y_min, 0ul); y_ind < std::min(y_max, dims.height); ++y_ind) {
            double x = (2 * x_ind + 1) / dims.height;
            double y = (2 * y_ind + 1) / dims.width;
            // std::array<CordType, 3> 
        }
    }
}

std::vector<Triangle> Triangle::intersect(const Plane& plane) const {
    auto check_is_inside = [&plane](const GeomPoint& p) {
        return plane.get_signed_distance(p) > 0;
    };
    std::array<bool, 3> check_results;
    std::transform(vertices_.begin(), vertices_.end(), check_results.begin(), check_is_inside);
    size_t inside_count = std::accumulate(check_results.begin(), check_results.end(), 0);
    if (inside_count == 0) {
        return {};
    }
    if (inside_count == 3) {
        return {*this};
    }
    std::vector<GeomPoint> inside;
    inside.reserve(inside_count + 2);
    // This may be inconsistent
    std::copy_if(vertices_.begin(), vertices_.end(), inside.begin(), check_is_inside);
    static constexpr std::array<std::pair<int, int>, 3> kEdges = {{{0, 1}, {0, 2}, {1, 2}}};
    for (const auto& [u, v] : kEdges) {
        if (check_results[u] != check_results[v]) {
            inside.push_back(Line(vertices_[u], vertices_[v]).intersect_plane(plane));
        }
    }
    assert(inside.size() == inside_count + 2);
    std::vector<Triangle> result;
    result.reserve(inside_count);
    for (size_t i = 2; i < inside.size(); ++i) {
        result.push_back(Triangle({inside[0], inside[1], inside[i]}, color_));
    }
    return result;
}

std::vector<Triangle::GeomPoint> Triangle::get_vertices() const {
    return std::vector<Triangle::GeomPoint>(vertices_.begin(), vertices_.end());
}

Triangle::CordType Triangle::edge_function(GeomPoint point, GeomPoint ver_1, GeomPoint ver_2) {
    auto v_1 = point - ver_1, v_2 = point - ver_2;
    return v_1.x() * v_2.y() - v_1.y() * v_2.x();
}

}  // namespace primitive

}  // namespace renderer

#include "primitive.h"

#include <algorithm>
#include <memory>
#include <numeric>
#include <tuple>

#include "Eigen/Geometry"
#include "geometry/algo.h"

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

Point::Point(GeomPoint position) : position_(position) {
}

void Point::transform_inplace(const Matrix<4, 4>& operation) {
    homogeneous_transform_inplace(position_, operation);
}

std::vector<Point> Point::intersect(const Plane& plane) const {
    if (plane.get_signed_distance(position_) > 0) {
        return {*this};
    } else {
        return {};
    }
}

std::array<Point::GeomPoint, Point::kVertexCount> Point::get_vertices() const {
    return {position_};
}

Segment::Segment(std::array<GeomPoint, kVertexCount> vertices) : vertices_(vertices) {
}

void Segment::transform_inplace(const Matrix<4, 4>& operation) {
    perform_transform_inplace<2>(vertices_, operation);
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
    auto outside_point =
        std::find_if(result.vertices_.begin(), result.vertices_.end(), check_is_outside);
    assert(outside_point != vertices_.end());
    *outside_point = Line(vertices_[0], vertices_[1]).intersect_plane(plane);
    return {result};
}

std::array<Segment::GeomPoint, Segment::kVertexCount> Segment::get_vertices() const {
    return vertices_;
}

Triangle::Triangle(std::array<GeomPoint, kVertexCount> vertices) : vertices_(vertices) {
}

void Triangle::transform_inplace(const Matrix<4, 4>& operation) {
    return perform_transform_inplace<3>(vertices_, operation);
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
    inside.resize(inside_count);
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
        result.push_back(Triangle({inside[i - 2], inside[i - 1], inside[i]}));
    }
    return result;
}

std::array<Triangle::GeomPoint, Triangle::kVertexCount> Triangle::get_vertices() const {
    return vertices_;
}

}  // namespace primitive

}  // namespace renderer

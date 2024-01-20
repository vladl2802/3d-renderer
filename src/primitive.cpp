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

Point::Point(std::array<GeomPoint, 1> vertices, RGBColor color)
    : color_(color), vertices_(vertices) {
}

void Point::transform_inplace(const Matrix<4, 4>& operation) {
    perform_transform_inplace<1>(vertices_, operation);
}

void Point::rasterize(Screen& screen) const {
    screen.put_pixel(vertices_[0], color_);
}

std::vector<Point> Point::intersect(const Plane& plane) const {
    if (plane.get_signed_distance(this->vertices_[0]) > 0) {
        return {*this};
    } else {
        return {};
    }
}

std::vector<Point::GeomPoint> Point::get_vertices() const {
    return std::vector<Point::GeomPoint>(vertices_.begin(), vertices_.end());
}

Segment::Segment(std::array<GeomPoint, 2> vertices, RGBColor color)
    : color_(color), vertices_(vertices) {
}

void Segment::transform_inplace(const Matrix<4, 4>& operation) {
    perform_transform_inplace<2>(vertices_, operation);
}

void Segment::rasterize(Screen& screen) const {
    double dif_x = vertices_[0].x() - vertices_[1].x();
    double dif_y = vertices_[0].y() - vertices_[1].y();
    std::array<GeomPoint, 2> inv_vertices;  // x, y are the same, z is inverted
    std::transform(vertices_.begin(), vertices_.end(), inv_vertices.begin(),
                   [](const GeomPoint& point) {
                       return GeomPoint{point.x(), point.y(), 1 / point.z()};
                   });
    Range<double> range;
    if (std::abs(dif_x) > std::abs(dif_y)) {
        range = Range(vertices_[0].x(), vertices_[1].x());
    } else {
        range = Range(vertices_[0].y(), vertices_[1].y());
    }
    LinearInterpolation li(range,
                           std::make_tuple(std::make_pair(inv_vertices[0], inv_vertices[1])));
    for (double pos = range.begin(); pos <= range.end(); ++pos) {
        auto [point] = li.interpolate(pos);
        screen.put_pixel(point, color_);
    }
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
    *outside_point = algo::intersect_plane_and_line(plane, Line(vertices_[0], vertices_[1]));
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
    std::array<GeomPoint, 3> inv_vertices;
    std::transform(vertices_.begin(), vertices_.end(), inv_vertices.begin(),
                   [](const GeomPoint& point) {
                       return GeomPoint{point.x(), point.y(), 1 / point.z()};
                   });
    std::sort(inv_vertices.begin(), inv_vertices.end(),
              [](const GeomPoint& lhs, const GeomPoint& rhs) { return lhs.y() < rhs.y(); });
    LinearInterpolation long_li(Range(inv_vertices[0].y(), inv_vertices[2].y()),
                                std::make_tuple(std::make_pair(inv_vertices[0], inv_vertices[2])));
    Range ver_range(inv_vertices[0].y(), inv_vertices[1].y());
    LinearInterpolation short_li(ver_range,
                                 std::make_tuple(std::make_pair(inv_vertices[0], inv_vertices[1])));
    for (double pos = ver_range.begin(); pos <= ver_range.end(); ++pos) {
        auto [point_long] = long_li.interpolate(pos);
        auto [point_short] = long_li.interpolate(pos);
        Range hor_range = Range(point_long.x(), point_short.x());
        LinearInterpolation hor(hor_range,
                                std::make_tuple(std::make_pair(point_long, point_short)));
        for (double pos = hor_range.begin(); hor_range.end(); ++pos) {
            auto [point] = hor.interpolate(pos);
            screen.put_pixel(point, color_);
        }
    }
    // need deduplicate this, but I'm not sure how for now
    ver_range = Range(inv_vertices[1].y(), inv_vertices[2].y());
    short_li = LinearInterpolation(
        ver_range, std::make_tuple(std::make_pair(inv_vertices[1], inv_vertices[2])));
    for (double pos = ver_range.begin(); pos <= ver_range.end(); ++pos) {
        auto [point_long] = long_li.interpolate(pos);
        auto [point_short] = long_li.interpolate(pos);
        Range hor_range = Range(point_long.x(), point_short.x());
        LinearInterpolation hor(hor_range,
                                std::make_tuple(std::make_pair(point_long, point_short)));
        for (double pos = hor_range.begin(); hor_range.end(); ++pos) {
            auto [point] = hor.interpolate(pos);
            screen.put_pixel(point, color_);
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
            inside.push_back(
                algo::intersect_plane_and_line(plane, Line(vertices_[u], vertices_[v])));
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

}  // namespace primitive

}  // namespace renderer

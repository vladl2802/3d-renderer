#pragma once

#include <cmath>
#include <memory>
#include <numeric>
#include <utility>

#include "Eigen/Geometry"
#include "primitive.h"
#include "screen.h"

namespace renderer {

class Rasterizer {
public:
    using IndexType = Screen::IndexType;
    using CordType = types::CordType;
    using Vector2 = types::Vector2;
    using Point = types::Point;
    using RGBColor = types::RGBColor;

    Rasterizer(std::unique_ptr<Screen> screen);

    template <primitive::IsPrimitive Primitive>
    void operator()(Primitive prim);

private:
    // not sure where to put this class
    class ProjectedVertexAtributes {
    public:
        ProjectedVertexAtributes() = default;
        explicit ProjectedVertexAtributes(Point point);

        CordType get_inverted_z() const;

        ProjectedVertexAtributes operator*(CordType val) const;
        ProjectedVertexAtributes operator*=(CordType val);
        ProjectedVertexAtributes operator+(const ProjectedVertexAtributes& other) const;
        ProjectedVertexAtributes operator+=(const ProjectedVertexAtributes& other);

    private:
        CordType inverted_z_;
    };

    struct BoundingRectangle {
        Screen::PixelPosition low;
        Screen::PixelPosition high;
    };

    using ProjectedPoint = Vector2;
    template <size_t N>
    using VerticesAttributes = std::array<ProjectedVertexAtributes, N>;
    template <size_t N>
    using ProjectedPoints = std::array<ProjectedPoint, N>;
    template <size_t N>
    using Vertices = std::array<Point, N>;

    ProjectedPoint rescale_to_screen(ProjectedPoint point) const;
    Screen::PixelPosition to_pixel_position(ProjectedPoint point) const;

    // don't know is it worth to create another struct here, because types in it says all
    template <size_t N>
    std::pair<ProjectedPoints<N>, VerticesAttributes<N>> split_and_rescale_vertices(
        const Vertices<N>& vertices) const;
    template <size_t N>
    BoundingRectangle get_bounding(const ProjectedPoints<N>& points) const;
    bool check_is_on_screen(ProjectedPoint point) const;

    // Segment specific
    static ProjectedVertexAtributes interpolate_over_line(CordType relation,
                                                          const VerticesAttributes<2>& attributes);

    // Triangle specific
    static CordType edge_function(ProjectedPoint point, ProjectedPoint ver_1, ProjectedPoint ver_2);
    static std::array<CordType, 3> calc_barycentric_coords(const ProjectedPoints<3>& vertices,
                                                           CordType area, ProjectedPoint point);
    static bool check_is_point_inside_triangle(std::array<CordType, 3> barycentric_coords);
    static ProjectedVertexAtributes interpolate_over_triangle(
        std::array<CordType, 3> barycentric_coords, const VerticesAttributes<3>& attributes);

    RGBColor color_ = RGBColor{100, 100, 100};  // This is temporal
    std::unique_ptr<Screen> screen_;
    Screen::Dimensions dims_;
};

template <>
inline void Rasterizer::operator()(primitive::Point prim) {
    auto [vert, attr] = split_and_rescale_vertices(prim.get_vertices());
    screen_->put_pixel(to_pixel_position(rescale_to_screen(vert[0])), attr[0].get_inverted_z(),
                       color_);
}

template <>
inline void Rasterizer::operator()(primitive::Segment prim) {
    // This is not efficient
    static const CordType BORDER = std::sqrt(2);
    const auto [points, attributes] = split_and_rescale_vertices(prim.get_vertices());
    const auto bounding = get_bounding(points);
    const Vector2 vec = points[1] - points[0];
    const CordType length = vec.norm();
    for (IndexType x_ind = bounding.low.x; x_ind <= bounding.high.x; ++x_ind) {
        for (IndexType y_ind = bounding.low.y; y_ind <= bounding.high.y; ++y_ind) {
            const ProjectedPoint pt((2 * x_ind + 1) / dims_.height, (2 * y_ind + 1) / dims_.width);
            const Vector2 tmp = pt - points[0];
            const CordType dist = std::abs(vec.cross(tmp) / length);
            if (dist < BORDER) {
                const auto attr = interpolate_over_line(tmp.norm() / length, attributes);
                screen_->put_pixel({x_ind, y_ind}, attr.get_inverted_z(), color_);
            }
        }
    }
}

template <>
inline void Rasterizer::operator()(primitive::Triangle prim) {
    // This is not efficient for thin triangles, so maybe add another implementation for such
    const auto [points, attributes] = split_and_rescale_vertices(prim.get_vertices());
    const auto bounding = get_bounding(points);
    const CordType area = edge_function(points[0], points[1], points[2]);
    for (IndexType x_ind = bounding.low.x; x_ind <= bounding.high.x; ++x_ind) {
        for (IndexType y_ind = bounding.low.y; y_ind <= bounding.high.y; ++y_ind) {
            const ProjectedPoint pt(x_ind + 0.5, y_ind + 0.5);
            std::array<CordType, 3> bar_coords = calc_barycentric_coords(points, area, pt);
            if (check_is_point_inside_triangle(bar_coords)) {
                const auto attr = interpolate_over_triangle(std::move(bar_coords), attributes);
                screen_->put_pixel({x_ind, y_ind}, attr.get_inverted_z(), color_);
            }
        }
    }
}

template <size_t N>
inline std::pair<Rasterizer::ProjectedPoints<N>, Rasterizer::VerticesAttributes<N>>
Rasterizer::split_and_rescale_vertices(const Vertices<N>& vertices) const {
    ProjectedPoints<N> points;
    VerticesAttributes<N> attrs;
    for (size_t i = 0; i < N; ++i) {
        ProjectedPoint point = vertices[i].template block<2, 1>(0, 0);
        assert(check_is_on_screen(point) && "vertex must lay on screen");
        points[i] = rescale_to_screen(point);
        attrs[i] = ProjectedVertexAtributes(vertices[i]);
    }
    return {points, attrs};
}

template <size_t N>
inline Rasterizer::BoundingRectangle Rasterizer::get_bounding(
    const ProjectedPoints<N>& points) const {
    BoundingRectangle initial{{dims_.width - 1, dims_.height - 1}, {0, 0}};
    auto bounding =
        std::accumulate(points.begin(), points.end(), std::move(initial),
                        [this, &points](BoundingRectangle bounding, ProjectedPoint point) {
                            auto pos = this->to_pixel_position(point);
                            bounding.low.x = std::min(bounding.low.x, pos.x);
                            bounding.high.x = std::max(bounding.high.x, pos.x);
                            bounding.low.y = std::min(bounding.low.y, pos.y);
                            bounding.high.y = std::max(bounding.high.y, pos.y);
                            return bounding;
                        });
    assert(bounding.low.x <= bounding.high.x && "bounding low x must be <= then high x");
    assert(bounding.low.y <= bounding.high.y && "bounding low y must be <= then high y");
    return bounding;
}
}  // namespace renderer

#include "rasterizer.h"

#include <algorithm>

#include "Eigen/Geometry"

namespace renderer {

Rasterizer::ProjectedVertexAtributes::ProjectedVertexAtributes(Point point)
    : inverted_z_(point.z()) {
}

Rasterizer::CordType Rasterizer::ProjectedVertexAtributes::get_inverted_z() const {
    return inverted_z_;
}

Rasterizer::ProjectedVertexAtributes Rasterizer::ProjectedVertexAtributes::operator*(
    CordType val) const {
    ProjectedVertexAtributes result(*this);
    result *= val;
    return result;
}

Rasterizer::ProjectedVertexAtributes Rasterizer::ProjectedVertexAtributes::operator*=(
    CordType val) {
    inverted_z_ *= val;
    return *this;
}

Rasterizer::ProjectedVertexAtributes Rasterizer::ProjectedVertexAtributes::operator+(
    const ProjectedVertexAtributes& other) const {
    ProjectedVertexAtributes result(*this);
    result += other;
    return result;
}

Rasterizer::ProjectedVertexAtributes Rasterizer::ProjectedVertexAtributes::operator+=(
    const ProjectedVertexAtributes& other) {
    inverted_z_ += other.inverted_z_;
    return *this;
}

Rasterizer::Rasterizer(std::unique_ptr<Screen> screen) : screen_(std::move(screen)) {
}

Rasterizer::ProjectedPoint Rasterizer::rescale_to_screen(ProjectedPoint point) const {
    return {(point.x() + 1) / 2 * dims_.width, (point.y() + 1) / 2 * dims_.height};
}

Screen::PixelPosition Rasterizer::to_pixel_position(ProjectedPoint point) const {
    return {std::clamp(static_cast<IndexType>(point.x()), 0ul, dims_.width),
            std::clamp(static_cast<IndexType>(point.y()), 0ul, dims_.height)};
}

bool Rasterizer::check_is_on_screen(ProjectedPoint point) const {
    return -1 <= point.x() && point.x() <= 1 && -1 <= point.y() && point.y() <= 1;
}

Rasterizer::ProjectedVertexAtributes Rasterizer::interpolate_over_line(
    CordType relation, const VerticesAttributes<2>& attributes) {
    return attributes[0] * relation + attributes[1] * (1 - relation);
}

Rasterizer::CordType Rasterizer::edge_function(ProjectedPoint point, ProjectedPoint ver_1,
                                               ProjectedPoint ver_2) {
    Vector2 v1 = point - ver_1, v2 = ver_2 - ver_1;
    return v1.cross(v2);
}

std::array<Rasterizer::CordType, 3> Rasterizer::calc_barycentric_coords(
    const ProjectedPoints<3>& vertices, CordType area, ProjectedPoint point) {
    std::array<CordType, 3> res;
    for (int i = 0; i < 3; ++i) {
        int j = (i + 1) % 3;
        res[i] = Rasterizer::edge_function(point, vertices[i], vertices[j]) / area;
    }
    return res;
}

bool Rasterizer::check_is_point_inside_triangle(std::array<CordType, 3> barycentric_coords) {
    return std::all_of(barycentric_coords.begin(), barycentric_coords.end(),
                       [](CordType w) { return w >= 0; });
}

Rasterizer::ProjectedVertexAtributes Rasterizer::interpolate_over_triangle(
    std::array<CordType, 3> barycentric_coords, const VerticesAttributes<3>& attributes) {
    Rasterizer::ProjectedVertexAtributes result;
    for (size_t i = 0; i < 3; ++i) {
        result += attributes[i] * barycentric_coords[i];
    }
    return result;
}

}  // namespace renderer

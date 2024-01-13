#include "primitive.h"

#include <memory>

#include "eigen3/Eigen/Geometry"

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

Primitive::Primitive(const Primitive& other)
    : data_(std::unique_ptr<primitive::Base>(other.data_->clone())) {
}

Primitive& Primitive::operator=(const Primitive& other) {
    Primitive temp(other);
    *this = std::move(temp);
    return *this;
}

Primitive Primitive::create_point(std::array<Point, 1> vertices, RGBColor color) {
    return Primitive(std::make_unique<primitive::Point>(vertices, color));
}

Primitive Primitive::create_segment(std::array<Point, 2> vertices, RGBColor color) {
    return Primitive(std::make_unique<primitive::Segment>(vertices, color));
}

Primitive Primitive::create_triangle(std::array<Point, 3> vertices, RGBColor color) {
    return Primitive(std::make_unique<primitive::Triangle>(vertices, color));
}

Primitive::Primitive(std::unique_ptr<primitive::Base>&& data) : data_(std::move(data)) {
}

Primitive Primitive::transform(const Matrix<4, 4>& operation) const {
    Primitive result(*this);
    result.transform_inplace(operation);
    return result;
}

void Primitive::transform_inplace(const Matrix<4, 4>& operation) {
    data_->transform_inplace(operation);
}

std::vector<Point> Primitive::get_vertices() const {
    return data_->get_vertices();
}

primitive::Point::Point(std::array<GeomPoint, 1> vertices, RGBColor color)
    : color_(color), vertices_(vertices) {
}

void primitive::Point::transform_inplace(const Matrix<4, 4>& operation) {
    perform_transform_inplace<1>(vertices_, operation);
}

std::vector<primitive::Base::GeomPoint> primitive::Point::get_vertices() const {
    return std::vector<primitive::Base::GeomPoint>(vertices_.begin(), vertices_.end());
}

std::unique_ptr<primitive::Base> primitive::Point::clone() const {
    return std::make_unique<primitive::Point>(*this);
}

primitive::Segment::Segment(std::array<GeomPoint, 2> vertices, RGBColor color)
    : color_(color), vertices_(vertices) {
}

void primitive::Segment::transform_inplace(const Matrix<4, 4>& operation) {
    perform_transform_inplace<2>(vertices_, operation);
}

std::vector<primitive::Base::GeomPoint> primitive::Segment::get_vertices() const {
    return std::vector<primitive::Base::GeomPoint>(vertices_.begin(), vertices_.end());
}

std::unique_ptr<primitive::Base> primitive::Segment::clone() const {
    return std::make_unique<primitive::Segment>(*this);
}

primitive::Triangle::Triangle(std::array<GeomPoint, 3> vertices, RGBColor color)
    : color_(color), vertices_(vertices) {
}

void primitive::Triangle::transform_inplace(const Matrix<4, 4>& operation) {
    return perform_transform_inplace<3>(vertices_, operation);
}

std::vector<primitive::Base::GeomPoint> primitive::Triangle::get_vertices() const {
    return std::vector<primitive::Base::GeomPoint>(vertices_.begin(), vertices_.end());
}

std::unique_ptr<primitive::Base> primitive::Triangle::clone() const {
    return std::make_unique<primitive::Triangle>(*this);
}

}  // namespace renderer

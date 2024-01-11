#include "primitive.h"

#include </usr/local/include/eigen3/Eigen/Geometry>
#include <memory>

using renderer::Point;
using renderer::Primitive;

namespace {
using renderer::Matrix;
using renderer::Vector4;

inline Point homogeneous_transform(const Point& position, const Matrix<4, 4>& operation) {
    Vector4 new_pos = operation * position.homogeneous();
    return new_pos.head<3>() / new_pos(3);
}

template <size_t N>
inline std::array<Point, N> perform_transform(const std::array<Point, N>& vertices,
                                              const Matrix<4, 4>& operation) {
    std::array<Point, N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = homogeneous_transform(vertices[i], operation);
    }
    return result;
}
}  // namespace

renderer::Primitive::Primitive(const Primitive& other)
    : data_(std::unique_ptr<primitive::Base>(other.data_->clone())) {
}

Primitive& renderer::Primitive::operator=(const Primitive& other) {
    data_.reset(other.data_->clone());
    return *this;
}

renderer::Primitive::Primitive(std::unique_ptr<primitive::Base>&& data) : data_(std::move(data)) {
}

Primitive Primitive::transform(const Matrix<4, 4>& operation) const {
    return data_->transform(operation);
}

std::vector<Point> renderer::Primitive::get_vertices() const {
    return data_->get_vertices();
}

renderer::primitive::Base::Base(RGBColor color, PrimitiveType type) : color_(color), type_(type) {
}

renderer::primitive::Point::Point(std::array<renderer::Point, 1> vertices, RGBColor color)
    : Base(color, PrimitiveType::Point), vertices_(vertices) {
}

Primitive renderer::primitive::Point::transform(const Matrix<4, 4>& operation) const {
    return Primitive(
        std::make_unique<primitive::Point>(perform_transform<1>(vertices_, operation), color_));
}

std::vector<renderer::Point> renderer::primitive::Point::get_vertices() const {
    return std::vector<renderer::Point>(vertices_.begin(), vertices_.end());
}

renderer::primitive::Base* renderer::primitive::Point::clone() const {
    return new Point(*this);
}

renderer::primitive::Segment::Segment(std::array<renderer::Point, 2> vertices, RGBColor color)
    : Base(color, PrimitiveType::Segment), vertices_(vertices) {
}

Primitive renderer::primitive::Segment::transform(const Matrix<4, 4>& operation) const {
    return Primitive(
        std::make_unique<primitive::Segment>(perform_transform<2>(vertices_, operation), color_));
}

std::vector<renderer::Point> renderer::primitive::Segment::get_vertices() const {
    return std::vector<renderer::Point>(vertices_.begin(), vertices_.end());
}

renderer::primitive::Base* renderer::primitive::Segment::clone() const {
    return new Segment(*this);
}

renderer::primitive::Triangle::Triangle(std::array<renderer::Point, 3> vertices, RGBColor color)
    : Base(color, PrimitiveType::Triangle), vertices_(vertices) {
}

Primitive renderer::primitive::Triangle::transform(const Matrix<4, 4>& operation) const {
    return Primitive(
        std::make_unique<primitive::Triangle>(perform_transform<3>(vertices_, operation), color_));
}

std::vector<renderer::Point> renderer::primitive::Triangle::get_vertices() const {
    return std::vector<renderer::Point>(vertices_.begin(), vertices_.end());
}

renderer::primitive::Base* renderer::primitive::Triangle::clone() const {
    return new Triangle(*this);
}

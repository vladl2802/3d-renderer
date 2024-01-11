#pragma once
#include <memory>

#include "geometry.h"

namespace renderer {
namespace primitive {
enum struct PrimitiveType { Point, Segment, Triangle };

class Base;
}  // namespace primitive

class Primitive {
public:
    Primitive() = default;
    Primitive(const Primitive& other);
    Primitive(Primitive&&) = default;
    Primitive& operator=(const Primitive& other);
    Primitive& operator=(Primitive&&) = default;

    Primitive(std::unique_ptr<primitive::Base>&& data);

    Primitive transform(const Matrix<4, 4>& operation) const;
    std::vector<Point> get_vertices() const;

private:
    std::unique_ptr<primitive::Base> data_;
};

namespace primitive {
// I will remove inheritance here when finish baseline
// Want to change from std::vector<std::unique_ptr<Base>> to std::tuple<std::vector<>, ...> with
// concept
class Base {
public:
    Base() = default;
    Base(const Base&) = default;
    Base(Base&&) = default;
    Base& operator=(const Base&) = default;
    Base& operator=(Base&&) = default;
    virtual ~Base() = default;

    Base(RGBColor color, PrimitiveType type);

    virtual Primitive transform(const Matrix<4, 4>& operation) const = 0;
    virtual std::vector<renderer::Point> get_vertices() const = 0;
    virtual Base* clone() const = 0;

    RGBColor color_;
    PrimitiveType type_;
};

class Point : public Base {
public:
    Point(std::array<renderer::Point, 1> vertices, RGBColor color);

    virtual Primitive transform(const Matrix<4, 4>& operation) const override;
    virtual std::vector<renderer::Point> get_vertices() const override;
    virtual Base* clone() const override;

private:
    std::array<renderer::Point, 1> vertices_;
};

class Segment : public Base {
public:
    Segment(std::array<renderer::Point, 2> vertices, RGBColor color);

    virtual Primitive transform(const Matrix<4, 4>& operation) const override;
    virtual std::vector<renderer::Point> get_vertices() const override;
    virtual Base* clone() const override;

private:
    std::array<renderer::Point, 2> vertices_;
};

class Triangle : public Base {
public:
    Triangle(std::array<renderer::Point, 3> vertices, RGBColor color);

    virtual Primitive transform(const Matrix<4, 4>& operation) const override;
    virtual std::vector<renderer::Point> get_vertices() const override;
    virtual Base* clone() const override;

private:
    std::array<renderer::Point, 3> vertices_;
};
}  // namespace primitive

}  // namespace renderer
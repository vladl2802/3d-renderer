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
    using Point = types::Point;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;
    using RGBColor = types::RGBColor;

    Primitive() = default;
    Primitive(const Primitive& other);
    Primitive(Primitive&&) noexcept = default;
    Primitive& operator=(const Primitive& other);
    Primitive& operator=(Primitive&&) noexcept = default;

    static Primitive create_point(std::array<Point, 1> vertices, RGBColor color);
    static Primitive create_segment(std::array<Point, 2> vertices, RGBColor color);
    static Primitive create_triangle(std::array<Point, 3> vertices, RGBColor color);

    Primitive transform(const Matrix<4, 4>& operation) const;
    void transform_inplace(const Matrix<4, 4>& operation);
    // Maybe change vector here to range or something similar, because it's not necessary to always
    // own the result
    std::vector<Point> get_vertices() const;

private:
    Primitive(std::unique_ptr<primitive::Base>&& data);

    std::unique_ptr<primitive::Base> data_;
};

namespace primitive {
// I will remove inheritance here when finish baseline
// Want to change from std::vector<std::unique_ptr<Base>> to std::tuple<std::vector<>, ...> with
// concept
class Base {
public:
    // I want to rename this, but I can't understand how for now
    using GeomPoint = Primitive::Point;
    template <int Rows, int Cols>
    using Matrix = Primitive::Matrix<Rows, Cols>;
    using RGBColor = Primitive::RGBColor;

    virtual ~Base() = default;
    
    virtual void transform_inplace(const Matrix<4, 4>& operation) = 0;
    virtual std::vector<GeomPoint> get_vertices() const = 0;
    // virtual void intersect() const = 0;
    // virtual void rasterize( Screen) const = 0;
    virtual std::unique_ptr<Base> clone() const = 0;
};

class Point : public Base {
public:
    Point(std::array<GeomPoint, 1> vertices, RGBColor color);

    void transform_inplace(const Matrix<4, 4>& operation) override;
    std::vector<GeomPoint> get_vertices() const override;
    std::unique_ptr<Base> clone() const override;

private:
    RGBColor color_;
    std::array<GeomPoint, 1> vertices_;
};

class Segment : public Base {
public:
    Segment(std::array<GeomPoint, 2> vertices, RGBColor color);

    void transform_inplace(const Matrix<4, 4>& operation) override;
    std::vector<GeomPoint> get_vertices() const override;
    std::unique_ptr<Base> clone() const override;

private:
    RGBColor color_;
    std::array<GeomPoint, 2> vertices_;
};

class Triangle : public Base {
public:
    Triangle(std::array<GeomPoint, 3> vertices, RGBColor color);

    void transform_inplace(const Matrix<4, 4>& operation) override;
    std::vector<GeomPoint> get_vertices() const override;
    std::unique_ptr<Base> clone() const override;

private:
    RGBColor color_;
    std::array<GeomPoint, 3> vertices_;
};
}  // namespace primitive

}  // namespace renderer

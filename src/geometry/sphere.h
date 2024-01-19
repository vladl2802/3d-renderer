#pragma once

#include "../geometry.h"

namespace renderer {

class Sphere {
public:
    using Point = types::Point;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;

    Sphere(Point center, double radius);

    const Point& get_center() const;
    void set_center(Point center);
    double get_radius() const;
    double get_squared_radius() const;
    void set_radius(double radius);

    Sphere transform(const Matrix<4, 4>& operation) const;
    void transform_inplace(const Matrix<4, 4>& operation);

private:
    Point center_;
    double radius_;
};

}  // namespace renderer

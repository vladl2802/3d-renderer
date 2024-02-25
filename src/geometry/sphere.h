#pragma once

#include "../geometry.h"

namespace renderer {

class Sphere {
public:
    using CordType = types::CordType;
    using Point = types::Point;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;

    Sphere(Point center, CordType radius);

    Point get_center() const;
    void set_center(Point center);
    CordType get_radius() const;
    CordType get_squared_radius() const;
    void set_radius(CordType radius);

    Sphere transform(const Matrix<4, 4>& operation) const;
    void transform_inplace(const Matrix<4, 4>& operation);

private:
    Point center_;
    CordType radius_;
};

}  // namespace renderer

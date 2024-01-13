#pragma once

#include "../geometry.h"

namespace renderer {

class Plane {
public:
    using Point = types::Point;
    using Vector3 = types::Vector3;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;

    Plane(Vector3 normal, double d);
    Plane(double a, double b, double c, double d);
    Plane(Vector3 normal, Point p);
    Plane(Point p1, Point p2, Point p3);

    double get_signed_distance(Point p) const;
    double get_distance(Point p) const;

    Plane transform(const Matrix<4, 4>& operation) const;
    void transform_inplace(const Matrix<4, 4>& operation);

private:
    Vector3 normal_;  // Normalized
    double d_;
};

}  // namespace renderer

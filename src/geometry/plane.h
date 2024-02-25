#pragma once

#include "../geometry.h"

namespace renderer {

class Plane {
public:
    using CordType = types::CordType;
    using Point = types::Point;
    using Vector3 = types::Vector3;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;

    Plane() = default;
    Plane(Vector3 normal, CordType d);
    Plane(CordType a, CordType b, CordType c, CordType d);
    Plane(Vector3 normal, Point p);
    Plane(Point p1, Point p2, Point p3);

    Vector3 get_normal() const;
    CordType get_signed_distance(Point p) const;
    CordType get_distance(Point p) const;

    Plane transform(const Matrix<4, 4>& operation) const;
    void transform_inplace(const Matrix<4, 4>& operation);

private:
    Vector3 normal_;  // Normalized
    CordType d_;
};

}  // namespace renderer

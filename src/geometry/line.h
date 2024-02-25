#pragma once

#include "../geometry.h"
#include "plane.h"

namespace renderer {

class Line {
public:
    using CordType = types::CordType;
    using Point = types::Point;
    using Vector3 = types::Vector3;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;

    Line(Point p1, Point p2);
    // Line(Point point, Vector direction);

    Point intersect_plane(const Plane& plane) const;

    Point get_point() const;
    Vector3 get_direction() const;
private:
    Point point_;
    Vector3 direction_;
};

}

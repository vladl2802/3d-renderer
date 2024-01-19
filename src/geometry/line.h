#pragma once

#include "../geometry.h"

namespace renderer {

class Line {
public:
    using Point = types::Point;
    using Vector3 = types::Vector3;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;

    Line(Point p1, Point p2);
    // Line(Point point, Vector direction);

    Point get_point() const;
    Vector3 get_direction() const;
private:
    Point point_;
    Vector3 direction_;
};

}

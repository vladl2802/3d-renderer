#pragma once

#include <memory>
#include <vector>

#include "geometry/plane.h"
#include "geometry/sphere.h"
#include "primitive.h"
#include "screen.h"

namespace renderer {

enum class BoundingCheckResult { OnPositiveSide, Intersects, OnNegativeSide };

class Object {
public:
    using Point = types::Point;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;
    using RGBColor = types::RGBColor;

    static Object create_triangle(Point p1, Point p2, Point p3);
    static Object create_segment(Point p1, Point p2);

    Object(Point position, const PrimitivesSet& primitives);

    std::vector<Point> get_vertices() const;
    BoundingCheckResult check_bounding(const Plane& plane) const;
    PrimitivesSet get_primitives() const;

private:
    Point position_;
    PrimitivesSet primitives_;

    Sphere bounding_;
};

}  // namespace renderer

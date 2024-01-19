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

    Object(Point position, PrimitivesSet primitives);

    std::vector<Point> get_vertices() const;
    BoundingCheckResult check_bounding(const Plane& plane) const;
    const PrimitivesSet& get_primitives() const;

private:
    Point position_;
    PrimitivesSet primitives_;

    Sphere bounding_;
};

}  // namespace renderer

#pragma once

#include <memory>
#include <vector>

#include "geometry/plane.h"
#include "geometry/sphere.h"
#include "primitive.h"

namespace renderer {

enum class BoundingCheckResult { OnPositiveSide, Intersects, OnNegativeSide };

class Object {
public:
    using Point = types::Point;

    Object(Point position, std::vector<Primitive> primitives);

    const std::vector<Primitive>& get_primitives() const;
    std::vector<Point> get_vertices() const;
    BoundingCheckResult check_bounding(const Plane& plane) const;

private:
    Point position_;
    std::vector<Primitive> primitives_;

    Sphere bounding_;
};

}  // namespace renderer

#include "object.h"

#include "geometry.h"
#include "geometry/algo.h"
#include "primitive.h"

namespace renderer {

using types::Point;

Object Object::create_triangle(Point p1, Point p2, Point p3, RGBColor color) {
    PrimitivesSet ps;
    ps.push(primitive::Triangle({p1, p2, p3}, color));
    return Object(Point(0, 0, 0), ps);
}

Object::Object(Point position, const PrimitivesSet& primitives)
    : position_(position),
      primitives_(primitives),
      bounding_(algo::get_bounding_sphere(primitives.get_vertices())) {
}

std::vector<Point> Object::get_vertices() const {
    return primitives_.get_vertices();
}

BoundingCheckResult Object::check_bounding(const Plane& plane) const {
    auto dist = plane.get_signed_distance(bounding_.get_center());
    if (std::abs(dist) < bounding_.get_radius()) {
        return BoundingCheckResult::Intersects;
    }
    return dist < 0 ? BoundingCheckResult::OnNegativeSide : BoundingCheckResult::OnPositiveSide;
}

const PrimitivesSet& Object::get_primitives() const {
    return primitives_;
}

}  // namespace renderer

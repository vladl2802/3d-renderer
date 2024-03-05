#include "object.h"

#include "geometry.h"
#include "geometry/algo.h"
#include "primitive.h"

namespace renderer {

using types::Point;

Object Object::create_triangle(Point p1, Point p2, Point p3) {
    PrimitivesSet ps;
    ps.push(primitive::Triangle({p1, p2, p3}));
    return Object(Point(0, 0, 0), ps);
}

Object Object::create_segment(Point p1, Point p2) {
    PrimitivesSet ps;
    ps.push(primitive::Segment({p1, p2}));
    return Object(Point(0, 0, 0), ps);
}

Object::Object(Point position, const PrimitivesSet& primitives)
    : position_(position),
      primitives_(primitives),
      bounding_(algo::get_bounding_sphere(primitives.get_vertices())) {
}

std::vector<Point> Object::get_vertices() const {
    auto result = primitives_.get_vertices();
    std::for_each(result.begin(), result.end(), [this](Point& point) { point += this->position_; });
    return result;
}

BoundingCheckResult Object::check_bounding(const Plane& plane) const {
    auto dist = plane.get_signed_distance(bounding_.get_center() + position_);
    if (std::abs(dist) < bounding_.get_radius()) {
        return BoundingCheckResult::Intersects;
    }
    return dist < 0 ? BoundingCheckResult::OnNegativeSide : BoundingCheckResult::OnPositiveSide;
}

PrimitivesSet Object::get_primitives() const {
    Matrix<4, 4> offset = Matrix<4, 4>::Identity();
    offset.block<3, 1>(0, 3) = position_;
    return primitives_.transform(offset);
}

}  // namespace renderer

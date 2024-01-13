#include "object.h"

#include "geometry.h"
#include "geometry/algo.h"
#include "primitive.h"

namespace renderer {

using types::Point;

namespace {
std::vector<Point> merge_primitives_vertices(const std::vector<Primitive>& primitives) {
    std::vector<Point> result;
    for (auto prim : primitives) {
        auto vers = prim.get_vertices();
        result.insert(result.end(), vers.begin(), vers.end());
    }
    return result;
}
}  // namespace

Object::Object(Point position, std::vector<Primitive> primitives)
    : position_(position),
      primitives_(primitives),
      bounding_(algo::get_bounding_sphere(merge_primitives_vertices(primitives))) {
}

std::vector<Point> Object::get_vertices() const {
    return merge_primitives_vertices(primitives_);
}

BoundingCheckResult Object::check_bounding(const Plane& plane) const {
    auto dist = plane.get_signed_distance(bounding_.get_center());
    if (std::abs(dist) < bounding_.get_radius()) {
        return BoundingCheckResult::Intersects;
    }
    return dist < 0 ? BoundingCheckResult::OnNegativeSide : BoundingCheckResult::OnPositiveSide;
}

}  // namespace renderer

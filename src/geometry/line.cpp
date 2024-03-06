#include "line.h"

namespace renderer {

Line::Line(Point p1, Point p2) : point_(p1), direction_(p2 - p1) {
}

Line::Point Line::intersect_plane(const Plane &plane) const {
    CordType num = -plane.get_signed_distance(point_);
    CordType den = direction_.dot(plane.get_normal());
    return num / den * direction_ + point_;
}

types::Point Line::get_point() const {
    return point_;
}

types::Vector3 Line::get_direction() const {
    return direction_;
}

}

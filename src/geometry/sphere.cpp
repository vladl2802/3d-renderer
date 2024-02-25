#include "sphere.h"

namespace renderer {

Sphere::Sphere(Point position, CordType radius) : center_(position), radius_(radius) {
    assert(radius > 0);
}

types::Point Sphere::get_center() const {
    return center_;
}

void Sphere::set_center(Point center) {
    center = center_;
}

Sphere::CordType Sphere::get_radius() const {
    return radius_;
}

Sphere::CordType renderer::Sphere::get_squared_radius() const {
    return radius_ * radius_;
}

void renderer::Sphere::set_radius(CordType radius) {
    assert(radius > 0);
    radius_ = radius;
}

}  // namespace renderer

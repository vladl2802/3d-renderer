#include "sphere.h"

namespace renderer {

Sphere::Sphere(Point position, double radius) : center_(position), radius_(radius) {
    assert(radius > 0);
}

const types::Point& Sphere::get_center() const {
    return center_;
}

void Sphere::set_center(Point center) {
    center = center_;
}

double Sphere::get_radius() const {
    return radius_;
}

double renderer::Sphere::get_squared_radius() const {
    return radius_ * radius_;
}

void renderer::Sphere::set_radius(double radius) {
    assert(radius > 0);
    radius_ = radius;
}

}  // namespace renderer

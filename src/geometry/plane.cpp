#include "plane.h"

#include </usr/local/include/eigen3/Eigen/Dense>
#include <cassert>

using renderer::Plane;

Plane::Plane(Vector3 normal, double d) : normal_(normal), d_(d) {
    normal_.normalize();
}

Plane::Plane(double a, double b, double c, double d) : Plane({a, b, c}, d) {
}

Plane::Plane(Vector3 normal, Point p) : Plane(normal, -normal.dot(p)) {
}

Plane::Plane(Point p1, Point p2, Point p3) : Plane((p2 - p1).cross(p3 - p1), p1) {
}

double Plane::get_signed_distance(Point p) const {
    return normal_.dot(p) + d_;
}

double Plane::get_distance(Point p) const {
    return std::abs(get_signed_distance(p));
}

Plane Plane::transform(const Matrix<4, 4>& operation) const {
    operation.inverse().transpose();
    Vector4 temp;
    temp.block<3, 1>(0, 0) = normal_;
    temp(3) = d_;
    temp = operation * temp;
    return Plane(temp.head<3>(), temp(3));
}
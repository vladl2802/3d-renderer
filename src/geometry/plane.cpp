#include "plane.h"

#include "Eigen/Dense"
#include <cassert>

namespace renderer {

using types::Vector4;

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
    Plane result(*this);
    result.transform_inplace(operation);
    return result;
    // operation.inverse().transpose();
    // Vector4 temp;
    // temp.block<3, 1>(0, 0) = normal_;
    // temp(3) = d_;
    // temp = operation * temp;
    // return Plane(temp.head<3>(), temp(3));
}

void Plane::transform_inplace(const Matrix<4, 4>& operation) {
    double d_temp = d_;
    d_ = operation(3, 3) * d_temp + operation.block<1, 3>(3, 0).dot(normal_);
    operation.inverse().transpose();
    normal_ = operation.block<3, 3>(0, 0) * normal_;
    normal_.array() += operation.block<3, 1>(0, 0).sum() * d_temp;
}

}  // namespace renderer

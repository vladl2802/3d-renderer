#include "plane.h"

#include <cassert>

#include "Eigen/Dense"

namespace renderer {

using types::Matrix;
using types::Vector4;

Plane::Plane(Vector3 normal, CordType d) : normal_(normal), d_(d) {
    assert(normal_.norm() > 0);
    normal_.normalize();
}

Plane::Plane(CordType a, CordType b, CordType c, CordType d) : Plane({a, b, c}, d) {
}

Plane::Plane(Vector3 normal, Point p) : Plane(normal, -normal.dot(p)) {
}

Plane::Plane(Point p1, Point p2, Point p3) : Plane((p2 - p1).cross(p3 - p1), p1) {
}

Plane::Vector3 Plane::get_normal() const {
    return normal_;
}

Plane::CordType Plane::get_signed_distance(Point p) const {
    return normal_.dot(p) + d_;
}

Plane::CordType Plane::get_distance(Point p) const {
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
    CordType d_temp = d_;
    Matrix<4, 4> op = operation.inverse().transpose();
    d_ = op(3, 3) * d_temp + op.block<1, 3>(3, 0).dot(normal_);
    normal_ = op.block<3, 3>(0, 0) * normal_;
    normal_ += d_temp * op.block<3, 1>(0, 3);
}

}  // namespace renderer

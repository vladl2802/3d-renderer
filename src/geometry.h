#pragma once

#include </usr/local/include/eigen3/Eigen/Core>

namespace renderer {

// enum class BoundaryRelationMask {
//     PositiveSide = 0b001,
//     Boundary = 0b010,
//     NegativeSide = 0b100,
//     NonPositiveSide = NegativeSide | Boundary,
//     NonNegativeSide = Boundary | PositiveSide
// };

using Vector3 = Eigen::Vector3d;
// Probably need class for right vector triple
using Point = Vector3;
using Vector4 = Eigen::Vector4d;
template <size_t Rows, size_t Cols>
using Matrix = Eigen::Matrix<double, Rows, Cols>;
// Probably need Transform<CoordSystem, TransformType> to typify transformations between different
// coordinate system (for e.x. in renderer between camera system and global system)
using RGBColor = Point;  // Move this to appropriate place

}  // namespace renderer

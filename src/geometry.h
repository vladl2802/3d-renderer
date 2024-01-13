#pragma once

#include "eigen3/Eigen/Core"

namespace renderer {

// enum class BoundaryRelationMask {
//     PositiveSide = 0b001,
//     Boundary = 0b010,
//     NegativeSide = 0b100,
//     NonPositiveSide = NegativeSide | Boundary,
//     NonNegativeSide = Boundary | PositiveSide
// };

namespace types {

using Vector3 = Eigen::Vector3d;
// Probably need class for right vector triple
using Point = Vector3;
using Vector4 = Eigen::Vector4d;
template <int Rows, int Cols>
using Matrix = Eigen::Matrix<double, Rows, Cols>;
// Probably need Transform<CoordSystem, TransformType> to typify transformations between different
// coordinate system (for e.x. in renderer between camera system and global system)
using RGBColor = Point;  // Move this to appropriate place

}  // namespace types

}  // namespace renderer

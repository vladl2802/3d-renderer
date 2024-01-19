#pragma once

#include <vector>

#include "../geometry.h"
#include "plane.h"
#include "sphere.h"
#include "line.h"

namespace renderer {

namespace algo {

Sphere get_bounding_sphere(const std::vector<types::Point>& points);
// Maybe move it in plane class
types::Point intersect_plane_and_line(const Plane& plane, const Line& line);

}  // namespace algo

}  // namespace renderer

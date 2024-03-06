#pragma once

#include <vector>

#include "../geometry.h"
#include "plane.h"
#include "sphere.h"
#include "line.h"

namespace renderer {

namespace algo {

Sphere get_bounding_sphere(const std::vector<types::Point>& points);

}  // namespace algo

}  // namespace renderer

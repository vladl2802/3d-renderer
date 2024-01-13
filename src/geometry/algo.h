#pragma once

#include <vector>

#include "../geometry.h"
#include "sphere.h"

namespace renderer {

namespace algo {

Sphere get_bounding_sphere(const std::vector<types::Point>& points);

}  // namespace algo

}  // namespace renderer

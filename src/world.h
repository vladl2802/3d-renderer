#pragma once

#include <vector>

#include "object.h"

namespace renderer {
class World {
    std::vector<Object> objects_;
};
}  // namespace renderer
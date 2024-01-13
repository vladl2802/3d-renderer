#pragma once

#include <vector>

#include "object.h"

namespace renderer {

class World {
public:
    const std::vector<Object>& get_objects() const;

private:
    std::vector<Object> objects_;
};

}  // namespace renderer

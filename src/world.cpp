#include "world.h"

namespace renderer {

const std::vector<Object>& renderer::World::get_objects() const {
    return objects_;
}

}  // namespace renderer

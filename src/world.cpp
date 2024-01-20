#include "world.h"

namespace renderer {

const std::vector<Object>& renderer::World::get_objects() const {
    return objects_;
}

void World::push_object(Object&& obj) {
    objects_.push_back(std::forward<Object>(obj));
}

}  // namespace renderer

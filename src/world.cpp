#include "world.h"

namespace renderer {

const std::vector<Object>& renderer::World::get_objects() const {
    return objects_;
}

void World::push_object(const Object& obj) {
    objects_.push_back(obj);
}

}  // namespace renderer

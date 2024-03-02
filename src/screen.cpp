#include "screen.h"

#include <limits>

namespace renderer {

Screen::Screen(Dimensions dims)
    : dims_(dims),
      depth_buffer_(dims.height,
                    std::vector<CordType>(dims.width, -std::numeric_limits<CordType>::infinity())),
      data_(dims.height, std::vector<RGBColor>(dims.width, {0, 0, 0})) {
}

void Screen::put_pixel(PixelPosition pos, CordType inv_z, RGBColor color) {
    assert(pos.x < dims_.width);
    assert(pos.y < dims_.height);
    if (inv_z < -1 || 1 < inv_z) {
        return;
    }
    if (depth_buffer_[pos.y][pos.x] < inv_z) {  // 1 / depth_buffer > 1 / inv_z
        depth_buffer_[pos.y][pos.x] = inv_z;
        data_[pos.y][pos.x] = color;
    }
}

const std::vector<std::vector<Screen::RGBColor>>& Screen::get_frame_buffer() const {
    return data_;
}

}  // namespace renderer

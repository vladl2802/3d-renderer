#include "screen.h"

#include <limits>

namespace renderer {

Screen::Screen(size_t height, size_t width)
    : height_(height),
      width_(width),
      depth_buffer_(height, std::vector<double>(width, std::numeric_limits<double>::infinity())),
      data_(height, std::vector<RGBColor>(width, {0, 0, 0})) {
}

}  // namespace renderer

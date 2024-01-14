#include "screen.h"

namespace renderer {

Screen::Screen(size_t height, size_t width)
    : height_(height), width_(width), data_(height, std::vector<RGBColor>(width, {0, 0, 0})) {
}

}  // namespace renderer

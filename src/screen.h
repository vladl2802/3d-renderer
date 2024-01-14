#pragma once

#include "geometry.h"

namespace renderer {

class Screen {
public:
    using RGBColor = types::RGBColor;

    Screen(size_t height, size_t width);

private:
    size_t height_;
    size_t width_;
    // Can be changed to dynarray
    std::vector<std::vector<RGBColor>> data_;
};

}  // namespace renderer

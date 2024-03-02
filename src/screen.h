#pragma once

#include <cassert>

#include "geometry.h"

namespace renderer {

class Screen {
public:
    using CordType = types::CordType;
    using IndexType = size_t;
    using Point = types::Point;
    using RGBColor = types::RGBColor;

    struct Dimensions {
        IndexType height;
        IndexType width;
    };

    struct PixelPosition {
        IndexType x;
        IndexType y;
    };

    Screen(Dimensions dims);

    void put_pixel(PixelPosition pos, CordType inv_z, RGBColor color);

    const std::vector<std::vector<RGBColor>>& get_frame_buffer() const;
    Dimensions get_dimensions() const;

private:
    Dimensions dims_;

    // Can be changed to dynarray
    std::vector<std::vector<CordType>> depth_buffer_;
    std::vector<std::vector<RGBColor>> data_;
};

}  // namespace renderer

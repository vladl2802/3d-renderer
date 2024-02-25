#pragma once

#include "geometry.h"

namespace renderer {

class Screen {
public:
    using CordType = types::CordType;
    using Point = types::Point;
    using RGBColor = types::RGBColor;

    Screen(size_t height, size_t width);

    inline void put_pixel(Point inv_point, RGBColor color);
    std::vector<std::vector<RGBColor>> get_frame_buffer() const;

private:
    size_t height_;
    size_t width_;

    // Can be changed to dynarray
    // Maybe merge depth and color in one cell to avoid cache misses
    std::vector<std::vector<CordType>> depth_buffer_;
    std::vector<std::vector<RGBColor>> data_;
};

inline void Screen::put_pixel(Point inv_point, RGBColor color) {
    auto x = inv_point.x();
    auto y = inv_point.y();
    auto inv_z = inv_point.z();
    if (x < -1 || 1 < x) {
        return;
    }
    if (y < -1 || 1 < y) {
        return;
    }
    if (-1 < inv_z && inv_z < 1) {
        return;
    }
    size_t num_x = (x + 1) / 2 * width_;
    size_t num_y = (y + 1) / 2 * height_;
    if (depth_buffer_[num_y][num_x] < inv_z) {  // 1 / depth_buffer > 1 / inv_z
        depth_buffer_[num_y][num_x] = inv_z;
        data_[num_y][num_x] = color;
    }
}

inline std::vector<std::vector<Screen::RGBColor>> Screen::get_frame_buffer() const {
    return data_;
}

}  // namespace renderer

#pragma once

#include "geometry.h"

namespace renderer {

class Screen {
public:
    struct Dimensions {
        size_t height;
        size_t width;
    };

    using CordType = types::CordType;
    using Point = types::Point;
    using RGBColor = types::RGBColor;

    Screen(Dimensions dims);

    Dimensions get_dimensions() const;

    // I don't really like that point with inverse to z also has type Point, but I'm not sure how to
    // fix that
    void put_pixel_in_screen_cords(Point inv_point, RGBColor color);
    void put_pixel_in_camera_cords(Point inv_point, RGBColor color);
    std::vector<std::vector<RGBColor>> get_frame_buffer() const;

private:
    void put_pixel(size_t x_ind, size_t y_ind, CordType z_inv, RGBColor color);
    bool check_boundaries(size_t x_ind, size_t y_ind, CordType z_inv);

    Dimensions dims_;

    // Can be changed to dynarray
    // Maybe merge depth and color in one cell to avoid cache misses
    std::vector<std::vector<double>> depth_buffer_;
    std::vector<std::vector<RGBColor>> data_;
};

}  // namespace renderer

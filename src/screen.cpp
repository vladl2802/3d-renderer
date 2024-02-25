#include "screen.h"

#include <cassert>
#include <limits>

namespace renderer {
Screen::Screen(Dimensions dims)
    : dims_(dims),
      depth_buffer_(dims.height,
                    std::vector<double>(dims.width, -std::numeric_limits<double>::infinity())),
      data_(dims.height, std::vector<RGBColor>(dims.width, {0, 0, 0})) {
}

Screen::Dimensions Screen::get_dimensions() const {
    return dims_;
}

void Screen::put_pixel_in_screen_cords(Point inv_point, RGBColor color) {
    put_pixel(inv_point.x(), inv_point.y(), inv_point.z(), color);
}

void Screen::put_pixel_in_camera_cords(Point inv_point, RGBColor color) {
    put_pixel((inv_point.x() + 1) / 2 * dims_.width, (inv_point.y() + 1) / 2 * dims_.height,
              inv_point.z(), color);
}

std::vector<std::vector<Screen::RGBColor>> Screen::get_frame_buffer() const {
    return data_;
}

void Screen::put_pixel(size_t x_pos, size_t y_pos, CordType z_inv, RGBColor color) {
    if (check_boundaries(x_pos, y_pos, z_inv)) {
        if (depth_buffer_[y_pos][x_pos] < z_inv) {  // 1 / depth_buffer > 1 / z_inv
            depth_buffer_[y_pos][x_pos] = z_inv;
            data_[y_pos][x_pos] = color;
        }
    }
}

bool Screen::check_boundaries(size_t x_ind, size_t y_ind, CordType z_inv) {
    bool x_check = 0 <= x_ind && x_ind < dims_.width;
    bool y_check = 0 <= y_ind && y_ind < dims_.height;
    bool z_check = -1 <= z_inv && z_inv <= 1;
    return x_check && y_check && z_check;
}

}  // namespace renderer

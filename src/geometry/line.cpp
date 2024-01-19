#include "line.h"

namespace renderer {

Line::Line(Point p1, Point p2) : point_(p1), direction_(p2 - p1) {
}

types::Point Line::get_point() const {
    return point_;
}

types::Vector3 Line::get_direction() const {
    return direction_;
}

}

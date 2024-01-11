#pragma once

#include "../geometry.h"

namespace renderer {

class Sphere {
public:
    Sphere(Point center, double radius);

    const Point& get_center() const;
    void set_center(Point center);
    double get_radius() const;
    double get_squared_radius() const;
    void set_radius(double radius);

private:
    Point center_;
    double radius_;
};
}
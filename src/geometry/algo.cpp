#include "algo.h"

#include <algorithm>
#include <cmath>
#include <random>

#include "sphere.h"

namespace renderer {

using types::CordType;
using types::Point;
using types::Vector3;

namespace {
Point find_farthest_point(const std::vector<Point>& points, Point pivot) {
    return *std::max_element(points.begin(), points.end(),
                             [&pivot](const Point& lhs, const Point& rhs) {
                                 return (lhs - pivot).norm() < (rhs - pivot).norm();
                             });
}
}  // namespace

// Ritter's algo
Sphere renderer::algo::get_bounding_sphere(const std::vector<Point>& points) {
    // Maybe make RandomGenerator singleton?
    std::random_device rd;
    std::mt19937 rnd(rd());
    const auto x = points[rnd() % points.size()];
    const auto y = find_farthest_point(points, x);
    const auto z = find_farthest_point(points, y);
    Sphere res((y + z) / 2.0, (y - z).norm() / 2.0);
    for (const auto& pt : points) {
        Vector3 dir = res.get_center() - pt;
        auto diff2 = dir.squaredNorm();
        auto rad2 = res.get_squared_radius();
        if (diff2 <= rad2) {
            CordType offset = std::sqrt(rad2 / diff2);
            Point new_center = (res.get_center() * (1.0 + offset) + pt * (1.0 - offset)) / 2.0;
            CordType new_radius = (dir.norm() + res.get_radius()) / 2.0;
            res = Sphere(new_center, new_radius);
        }
    }
    return res;
}

}  // namespace renderer

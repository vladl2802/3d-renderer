#include "algo.h"

#include <algorithm>
#include <cmath>
#include <random>

#include "sphere.h"

using renderer::Sphere;

// Ritter's algo
Sphere renderer::algo::get_bounding_sphere(const std::vector<Point>& points) {
    // Maybe make RandomGenerator singleton?
    std::random_device rd;
    std::mt19937 rnd(rd());
    const auto x = points[rnd() % points.size()];
    const auto y = *std::max_element(points.begin(), points.end(),
                                     [&x](const Point& lhs, const Point& rhs) -> bool {
                                         return (lhs - x).norm() < (rhs - x).norm();
                                     });
    const auto z = *std::max_element(points.begin(), points.end(),
                                     [&y](const Point& lhs, const Point& rhs) -> bool {
                                         return (lhs - y).norm() < (rhs - y).norm();
                                     });
    Sphere res((y + z) / 2, (y - z).norm() / 2);
    for (const auto& pt : points) {
        const auto dir = res.get_center() - pt;
        auto diff2 = dir.squaredNorm();
        auto rad2 = res.get_squared_radius();
        if (diff2 <= rad2) {
            double offset = std::sqrt(rad2 / diff2);
            Point new_center = (res.get_center() * (1 + offset) + pt * (1 - offset)) / 2;
            double new_radius = (dir.norm() + res.get_radius()) / 2;
            res = Sphere(new_center, new_radius);
        }
    }
    return res;
}
#pragma once

#include <concepts>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

#include "geometry.h"
#include "geometry/plane.h"
#include "screen.h"

namespace renderer {

namespace primitive {

// class PrimitivesSet;

template <typename T>
concept Transformable = requires(T prim, const types::Matrix<4, 4>& operation) {
    { prim.transform_inplace(operation) } -> std::same_as<void>;
    // { prim.transform(operation) } -> std::same_as<T>;
};

template <typename T>
// Maybe replace here and later reference with pointer
concept Rasterizable = requires(const T& prim, Screen& screen) {
    { prim.rasterize(screen) } -> std::same_as<void>;
};

template <typename T>
concept Intersectable = requires(const T& prim, const Plane& plane) {
    { prim.intersect(plane) } -> std::same_as<std::vector<T>>;
};

// This maybe be preferable, because it not limits result to contain the same primitives and doesn't
// create useless temporary vector but both solutions seems to be terrible performance wise
// template <typename T>
// concept Intersectable = requires(T prim, const Plane& plane, PrimitivesSet& cont) {
//     { prim.intersect(plane, cont) } -> std::same_as<void>;
// };

template <typename T>
concept IsPrimitive = std::copyable<T> && Transformable<T> && Intersectable<T> && Rasterizable<T> &&
                      requires(T prim) {
                          { prim.get_vertices() } -> std::same_as<std::vector<types::Point>>;
                      };

// Move it appropriate place
template <typename T, typename... U>
concept SameAsAny = (... || std::same_as<T, U>);

// Move it appropriate place
template <typename F, typename... U>
concept InvocableForAll =
    (... && (std::invocable<F, U> || std::invocable<F, U&> || std::invocable<F, const U&>));

class PrimitiveBase {
public:
    // I want to rename this, but I can't understand how for now
    using GeomPoint = types::Point;
    template <int Rows, int Cols>
    using Matrix = types::Matrix<Rows, Cols>;
    using RGBColor = types::RGBColor;
};

template <primitive::IsPrimitive... Primitives>
class PrimitivesSet {
public:
    using GeomPoint = PrimitiveBase::GeomPoint;
    template <int Rows, int Cols>
    using Matrix = PrimitiveBase::Matrix<Rows, Cols>;
    using RGBColor = PrimitiveBase::RGBColor;

    template <SameAsAny<Primitives...> T>
    void push(const T& value);

    void transform_inplace(const Matrix<4, 4>& operation);
    PrimitivesSet transform(const Matrix<4, 4>& operation) const;

    void rasterize(Screen& screen) const;
    void intersect_and_rasterize(const std::array<Plane, 6>& bounding, Screen& screen) const;

    // Change vector here to iterable proxy and maybe allocate all vertices in one container
    std::vector<GeomPoint> get_vertices() const;

private:
    // Need apply method that will call function on every std::vector contained in data_ to properly
    // use std::algorithms, but for now for_each method is enough

    template <InvocableForAll<Primitives&...> F>
    void for_each(F&& func);
    template <InvocableForAll<const Primitives&...> F>
    void for_each(F&& func) const;

    std::tuple<std::vector<Primitives>...> data_;
};

class Point : public PrimitiveBase {
public:
    Point(GeomPoint position, RGBColor color);

    void transform_inplace(const Matrix<4, 4>& operation);
    void rasterize(Screen& screen) const;
    std::vector<Point> intersect(const Plane& plane) const;
    std::vector<GeomPoint> get_vertices() const;

private:
    RGBColor color_;
    GeomPoint position_;
};

class Segment : public PrimitiveBase {
public:
    Segment(std::array<GeomPoint, 2> vertices, RGBColor color);

    void transform_inplace(const Matrix<4, 4>& operation);
    void rasterize(Screen& screen) const;
    std::vector<Segment> intersect(const Plane& plane) const;
    std::vector<GeomPoint> get_vertices() const;

private:
    RGBColor color_;
    std::array<GeomPoint, 2> vertices_;
};

class Triangle : public PrimitiveBase {
public:
    Triangle(std::array<GeomPoint, 3> vertices, RGBColor color);

    void transform_inplace(const Matrix<4, 4>& operation);
    void rasterize(Screen& screen) const;
    std::vector<Triangle> intersect(const Plane& plane) const;
    std::vector<GeomPoint> get_vertices() const;

private:
    RGBColor color_;
    std::array<GeomPoint, 3> vertices_;
};

template <primitive::IsPrimitive... Primitives>
template <SameAsAny<Primitives...> T>
inline void PrimitivesSet<Primitives...>::push(const T& value) {
    std::get<std::vector<T>>(data_).push_back(value);
}

template <primitive::IsPrimitive... Primitives>
template <InvocableForAll<Primitives&...> F>
inline void PrimitivesSet<Primitives...>::for_each(F&& func) {
    std::apply(
        [func = std::forward<decltype(func)>(func)](auto&... vectors) {
            auto temp = [func = std::forward<decltype(func)>(func)](auto& primitives_vector) {
                std::for_each(primitives_vector.begin(), primitives_vector.end(), func);
            };
            (temp(vectors), ...);
        },
        data_);
}

template <primitive::IsPrimitive... Primitives>
template <InvocableForAll<const Primitives&...> F>
inline void PrimitivesSet<Primitives...>::for_each(F&& func) const {
    std::apply(
        [func = std::forward<decltype(func)>(func)](const auto&... vectors) {
            auto temp = [func = std::forward<decltype(func)>(func)](const auto& primitives_vector) {
                std::for_each(primitives_vector.begin(), primitives_vector.end(), func);
            };
            (temp(vectors), ...);
        },
        data_);
}

template <primitive::IsPrimitive... Primitives>
inline void PrimitivesSet<Primitives...>::transform_inplace(const Matrix<4, 4>& operation) {
    for_each([&operation](SameAsAny<Primitives...> auto& primitive) {
        primitive.transform_inplace(operation);
    });
}

template <primitive::IsPrimitive... Primitives>
inline PrimitivesSet<Primitives...> PrimitivesSet<Primitives...>::transform(
    const Matrix<4, 4>& operation) const {
    PrimitivesSet result(*this);
    result.transform_inplace(operation);
    return result;
}

template <primitive::IsPrimitive... Primitives>
inline void PrimitivesSet<Primitives...>::rasterize(Screen& screen) const {
    for_each(
        [&screen](const SameAsAny<Primitives...> auto& primitive) { primitive.rasterize(screen); });
}

template <primitive::IsPrimitive... Primitives>
inline void PrimitivesSet<Primitives...>::intersect_and_rasterize(
    const std::array<Plane, 6>& bounding, Screen& screen) const {
    for_each([&bounding, &screen](const SameAsAny<Primitives...> auto& primitive) {
        using CurrentPrimitiveType = std::decay_t<decltype(primitive)>;
        // I really hate this implementation, will fix it later
        std::vector<CurrentPrimitiveType> partition = {primitive};
        for (const auto& plane : bounding) {
            std::vector<CurrentPrimitiveType> new_partition;
            for (const auto& old : partition) {
                auto temp = old.intersect(plane);
                new_partition.insert(new_partition.end(), temp.begin(), temp.end());
            }
            partition = new_partition;
        }
        std::for_each(
            partition.begin(), partition.end(),
            [&screen](const SameAsAny<Primitives...> auto& prim) { prim.rasterize(screen); });
    });
}

template <primitive::IsPrimitive... Primitives>
inline std::vector<typename PrimitivesSet<Primitives...>::GeomPoint>
PrimitivesSet<Primitives...>::get_vertices() const {
    std::vector<GeomPoint> result;
    for_each([&result](const SameAsAny<Primitives...> auto& primitive) {
        auto temp = primitive.get_vertices();
        result.insert(result.end(), temp.begin(), temp.end());
    });
    return result;
}

}  // namespace primitive

template <primitive::IsPrimitive T>
struct Checker {};

using PointKek = Checker<primitive::Point>;
using SegmentKek = Checker<primitive::Segment>;
using TriangleKek = Checker<primitive::Triangle>;

using PrimitivesSet =
    primitive::PrimitivesSet<primitive::Point, primitive::Segment, primitive::Triangle>;

}  // namespace renderer

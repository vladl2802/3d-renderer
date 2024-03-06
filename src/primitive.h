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

// template <typename T>
// concept Rasterizable = requires(const T& prim, Screen& screen) {
//     { prim.rasterize(screen) } -> std::same_as<void>;
// };

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
concept IsPrimitive = std::copyable<T> && Transformable<T> && Intersectable<T>;  // && Iterable<T>

// Move it appropriate place
template <typename T, typename... U>
concept SameAsAny = (... || std::same_as<T, U>);

// Move it appropriate place
template <typename F, typename... U>
concept InvocableForAll = (... && std::invocable<F, U>);

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

    using BoundingBox = std::array<Plane, 6>;

    template <SameAsAny<Primitives...> T>
    void push(T value);
    template <InvocableForAll<Primitives&...> F>
    void for_each(F&& func);
    template <InvocableForAll<const Primitives&...> F>
    void for_each(F&& func) const;

    void transform_inplace(const Matrix<4, 4>& operation);
    PrimitivesSet transform(const Matrix<4, 4>& operation) const;

    // void intersect_inplace(const BoundingBox& bounding);
    PrimitivesSet intersect(const BoundingBox& bounding) const;

    // Change vector here to iterable
    std::vector<GeomPoint> get_vertices() const;

private:
    // Need apply method that will call function on every std::vector contained in data_ to properly
    // use std::algorithms, but for now for_each method is enough

    std::tuple<std::vector<Primitives>...> data_;
};

class Point : public PrimitiveBase {
public:
    static constexpr size_t kVertexCount = 1;

    Point(GeomPoint position);

    void transform_inplace(const Matrix<4, 4>& operation);
    std::vector<Point> intersect(const Plane& plane) const;
    std::array<GeomPoint, kVertexCount> get_vertices() const;

private:
    GeomPoint position_;
};

class Segment : public PrimitiveBase {
public:
    static constexpr size_t kVertexCount = 2;

    Segment(std::array<GeomPoint, kVertexCount> vertices);

    void transform_inplace(const Matrix<4, 4>& operation);
    std::vector<Segment> intersect(const Plane& plane) const;
    std::array<GeomPoint, kVertexCount> get_vertices() const;

private:
    std::array<GeomPoint, kVertexCount> vertices_;
};

class Triangle : public PrimitiveBase {
public:
    static constexpr size_t kVertexCount = 3;

    Triangle(std::array<GeomPoint, kVertexCount> vertices);

    void transform_inplace(const Matrix<4, 4>& operation);
    std::vector<Triangle> intersect(const Plane& plane) const;
    std::array<GeomPoint, kVertexCount> get_vertices() const;

private:
    std::array<GeomPoint, kVertexCount> vertices_;
};

template <primitive::IsPrimitive... Primitives>
template <SameAsAny<Primitives...> T>
inline void PrimitivesSet<Primitives...>::push(T value) {
    std::get<std::vector<T>>(data_).push_back(std::forward<T>(value));
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
inline PrimitivesSet<Primitives...> PrimitivesSet<Primitives...>::intersect(
    const BoundingBox& bounding) const {
    PrimitivesSet<Primitives...> result;
    for_each([&result, &bounding](const SameAsAny<Primitives...> auto& primitive) {
        using PrimitiveType = std::decay_t<decltype(primitive)>;
        // I really hate this implementation
        std::vector<PrimitiveType> partition = {primitive};
        for (const auto& plane : bounding) {
            std::vector<PrimitiveType> new_partition;
            for (const auto& old : partition) {
                auto temp = old.intersect(plane);
                new_partition.insert(new_partition.end(), std::make_move_iterator(temp.begin()),
                                     std::make_move_iterator(temp.end()));
            }
            partition = std::move(new_partition);
        }
        std::for_each(std::make_move_iterator(partition.begin()),
                      std::make_move_iterator(partition.end()),
                      [&result](PrimitiveType&& prim) { result.push(prim); });
    });
    return result;
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

using PrimitivesSet =
    primitive::PrimitivesSet<primitive::Point, primitive::Segment, primitive::Triangle>;

}  // namespace renderer

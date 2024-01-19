#pragma once

#include <array>
#include <utility>

#include "../geometry.h"

namespace renderer {

template <typename T>
class Range {
public:
    using Type = T;

    Range() = default;
    Range(T a, T b);

    T begin() const;
    T end() const;
    T len() const;
private:

    T begin_;
    T end_;
};

// This class is really questionable, but I didn't come up with better
// Maybe add concept for value to have +- binary operators and can be */ by Type
template <typename... Values>
class LinearInterpolation {
public:
    using Type = double;
    template <typename T>
    using RangeType = Range<T>;

    // LinearInterpolation() = default;
    LinearInterpolation(RangeType<Type> parameter, std::tuple<std::pair<Values, Values>...> values);

    auto interpolate(Type value) -> std::tuple<Values...> const;

private:
    Type parameter_base_;
    std::tuple<std::pair<Values, Values>...> lines_;
};

template <typename T>
inline Range<T>::Range(T a, T b) : begin_(std::min(a, b)), end_(std::max(a, b)) {
}

template <typename T>
inline T Range<T>::begin() const {
    return begin_;
}

template <typename T>
inline T Range<T>::end() const {
    return end_;
}

template <typename T>
inline T Range<T>::len() const {
    return begin_ - end_;
}

template <typename... Values>
inline LinearInterpolation<Values...>::LinearInterpolation(
    RangeType<Type> parameter, std::tuple<std::pair<Values, Values>...> values)
    : parameter_base_(parameter.begin()) {
    Type diff = parameter.len();
    std::apply(
        [&parameter, diff, this](const auto&... ranges) {
            (
                [&parameter, diff, this](const auto& range) {
                    using T = std::decay_t<decltype(range)>::first_type;
                    std::get<std::pair<T, T>>(lines_) = {range.first, range.second / diff};
                }(ranges),
                ...);
        },
        values);
}

template <typename... Values>
inline auto LinearInterpolation<Values...>::interpolate(Type value) -> std::tuple<Values...> const {
    std::tuple<Values...> result;
    Type diff = value - parameter_base_;
    std::apply(
        [&result, diff](const auto&... ranges) {
            (
                [&result, diff](const auto& range) {
                    using T = std::decay_t<decltype(range)>::first_type;
                    std::get<T>(result) = range.first + range.second * diff;
                }(ranges),
                ...);
        },
        lines_);
    return result;
}


}  // namespace renderer

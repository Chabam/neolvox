#ifndef LVOX_TYPES_HPP
#define LVOX_TYPES_HPP

#include <Eigen/Eigen>
#include <concepts>

namespace lvox
{

template <typename T>
concept Point = requires(const T& pts) {
    { pts.x() } -> std::convertible_to<double>;
    { pts.y() } -> std::convertible_to<double>;
    { pts.z() } -> std::convertible_to<double>;
    { pts.gps_time() } -> std::convertible_to<double>;
};

using Vector = Eigen::Vector3d;

template <typename T, typename V>
concept PointCloud = std::ranges::range<T> && Point<V> &&
                     std::same_as<std::ranges::range_value_t<T>, V> && requires(const T& r) {
                         { r.size() } -> std::convertible_to<size_t>;
                     };

using Index3D = std::array<int, 3>;
} // namespace lvox

#endif // LVOX_TYPES_HPP

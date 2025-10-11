#ifndef LVOX_BOUNDS_HPP
#define LVOX_BOUNDS_HPP

#include <lvox/types.hpp>

namespace lvox
{

struct Bounds
{
    Bounds();
    Bounds(const Bounds& other);
    Bounds(Bounds&& other);
    Bounds(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z);
    auto operator=(const Bounds& other) -> Bounds&;
    auto operator=(Bounds&& other) -> Bounds&;

    auto grow(double x, double y, double z) -> void;
    auto grow(const Bounds& other) -> void;
    auto contains(double x, double y, double z) const -> bool;
    auto contains(const Bounds& other) const -> bool;
    auto operator==(const Bounds& other) const -> bool;

    double m_min_x;
    double m_max_x;
    double m_min_y;
    double m_max_y;
    double m_min_z;
    double m_max_z;
};

} // namespace lvox

#endif // LVOX_BOUNDS_HPP

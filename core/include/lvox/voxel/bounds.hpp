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
    Bounds& operator=(const Bounds& other);
    Bounds& operator=(Bounds&& other);

    void grow(double x, double y, double z);
    void grow(const Bounds& other);
    bool contains(double x, double y, double z) const;
    bool contains(const Bounds& other) const;
    bool operator==(const Bounds& other) const;

    double m_min_x;
    double m_max_x;
    double m_min_y;
    double m_max_y;
    double m_min_z;
    double m_max_z;
};

} // namespace lvox

#endif // LVOX_BOUNDS_HPP

#ifndef LVOX_BOUNDS_HPP
#define LVOX_BOUNDS_HPP

#include <lvox/types.hpp>

namespace lvox
{

template <typename T>
struct Bounds
{
    Bounds()
        : m_min_x{std::numeric_limits<T>::max()}
        , m_max_x{-std::numeric_limits<T>::max()}
        , m_min_y{std::numeric_limits<T>::max()}
        , m_max_y{-std::numeric_limits<T>::max()}
        , m_min_z{std::numeric_limits<T>::max()}
        , m_max_z{-std::numeric_limits<T>::max()}
    {
    }

    Bounds(const Bounds& other)
        : m_min_x{other.m_min_x}
        , m_max_x{other.m_max_x}
        , m_min_y{other.m_min_y}
        , m_max_y{other.m_max_y}
        , m_min_z{other.m_min_z}
        , m_max_z{other.m_max_z}
    {
    }

    Bounds(Bounds&& other)
        : m_min_x{std::move(other.m_min_x)}
        , m_max_x{std::move(other.m_max_x)}
        , m_min_y{std::move(other.m_min_y)}
        , m_max_y{std::move(other.m_max_y)}
        , m_min_z{std::move(other.m_min_z)}
        , m_max_z{std::move(other.m_max_z)}
    {
    }

    Bounds(T min_x, T max_x, T min_y, T max_y, T min_z, T max_z)
        : m_min_x{min_x}
        , m_max_x{max_x}
        , m_min_y{min_y}
        , m_max_y{max_y}
        , m_min_z{min_z}
        , m_max_z{max_z}
    {
    }

    Bounds& operator=(const Bounds& other)
    {
        m_min_x = other.m_min_x;
        m_max_x = other.m_max_x;
        m_min_y = other.m_min_y;
        m_max_y = other.m_max_y;
        m_min_z = other.m_min_z;
        m_max_z = other.m_max_z;

        return *this;
    }

    Bounds& operator=(Bounds&& other)
    {
        m_min_x = std::move(other.m_min_x);
        m_max_x = std::move(other.m_max_x);

        m_min_y = std::move(other.m_min_y);
        m_max_y = std::move(other.m_max_y);

        m_min_z = std::move(other.m_min_z);
        m_max_z = std::move(other.m_max_z);

        return *this;
    }

    void grow(T x, T y, T z)
    {

        m_min_x = std::min(x, m_min_x);
        m_max_x = std::max(x, m_max_x);

        m_min_y = std::min(y, m_min_y);
        m_max_y = std::max(y, m_max_y);

        m_min_z = std::min(z, m_min_z);
        m_max_z = std::max(z, m_max_z);
    }

    void grow(const Bounds& other)
    {
        grow(other.m_min_x, other.m_min_y, other.m_min_z);
        grow(other.m_max_x, other.m_max_y, other.m_max_z);
    }

    bool contains(T x, T y, T z) const
    {
        const auto is_within_dimension = [](double val, double min, double max) -> bool {
            return val >= min && val <= max;
        };

        return is_within_dimension(x, m_min_x, m_max_x) &&
               is_within_dimension(y, m_min_y, m_max_y) && //
               is_within_dimension(z, m_min_z, m_max_z);
    }

    bool contains(const Bounds& other) const
    {
        return contains(other.m_min_x, other.m_min_y, other.m_min_z) &&
               contains(other.m_max_x, other.m_max_y, other.m_max_z);
    }

    bool operator==(const Bounds& other) const
    {
        return m_min_x == other.m_min_x && m_max_x == other.m_max_x && m_min_y == other.m_min_y &&
               m_max_y == other.m_max_y && m_min_z == other.m_min_z && m_max_z == other.m_max_z;
    }

    T m_min_x;
    T m_max_x;
    T m_min_y;
    T m_max_y;
    T m_min_z;
    T m_max_z;
};

} // namespace lvox

#endif // LVOX_BOUNDS_HPP

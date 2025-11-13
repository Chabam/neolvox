#include <algorithm>
#include <limits>

#include <lvox/voxel/bounds.hpp>

namespace lvox
{

Bounds::Bounds()
    : m_min_x{std::numeric_limits<double>::max()}
    , m_max_x{-std::numeric_limits<double>::max()}
    , m_min_y{std::numeric_limits<double>::max()}
    , m_max_y{-std::numeric_limits<double>::max()}
    , m_min_z{std::numeric_limits<double>::max()}
    , m_max_z{-std::numeric_limits<double>::max()}
{
}

Bounds::Bounds(const Bounds& other)
    : m_min_x{other.m_min_x}
    , m_max_x{other.m_max_x}
    , m_min_y{other.m_min_y}
    , m_max_y{other.m_max_y}
    , m_min_z{other.m_min_z}
    , m_max_z{other.m_max_z}
{
}

Bounds::Bounds(Bounds&& other)
    : m_min_x{std::move(other.m_min_x)}
    , m_max_x{std::move(other.m_max_x)}
    , m_min_y{std::move(other.m_min_y)}
    , m_max_y{std::move(other.m_max_y)}
    , m_min_z{std::move(other.m_min_z)}
    , m_max_z{std::move(other.m_max_z)}
{
}

Bounds::Bounds(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z)
    : m_min_x{min_x}
    , m_max_x{max_x}
    , m_min_y{min_y}
    , m_max_y{max_y}
    , m_min_z{min_z}
    , m_max_z{max_z}
{
}

Bounds& Bounds::operator=(const Bounds& other)
{
    m_min_x = other.m_min_x;
    m_max_x = other.m_max_x;
    m_min_y = other.m_min_y;
    m_max_y = other.m_max_y;
    m_min_z = other.m_min_z;
    m_max_z = other.m_max_z;

    return *this;
}

Bounds& Bounds::operator=(Bounds&& other)
{
    m_min_x = std::move(other.m_min_x);
    m_max_x = std::move(other.m_max_x);
    m_min_y = std::move(other.m_min_y);
    m_max_y = std::move(other.m_max_y);
    m_min_z = std::move(other.m_min_z);
    m_max_z = std::move(other.m_max_z);

    return *this;
}

void Bounds::grow(double x, double y, double z)
{
    m_min_x = std::min(x, m_min_x);
    m_max_x = std::max(x, m_max_x);

    m_min_y = std::min(y, m_min_y);
    m_max_y = std::max(y, m_max_y);

    m_min_z = std::min(z, m_min_z);
    m_max_z = std::max(z, m_max_z);
}

void Bounds::grow(const Bounds& other)
{
    grow(other.m_min_x, other.m_min_y, other.m_min_z);
    grow(other.m_max_x, other.m_max_y, other.m_max_z);
}

bool Bounds::contains(double x, double y, double z) const
{
    const auto is_within_dimension = [](double val, double min, double max) -> bool {
        return val >= min && val <= max;
    };

    return is_within_dimension(x, m_min_x, m_max_x) &&
           is_within_dimension(y, m_min_y, m_max_y) &&
           is_within_dimension(z, m_min_z, m_max_z);
}

bool Bounds::contains(const Bounds& other) const
{
    return contains(other.m_min_x, other.m_min_y, other.m_min_z) &&
           contains(other.m_max_x, other.m_max_y, other.m_max_z);
}

bool Bounds::operator==(const Bounds& other) const
{
    return m_min_x == other.m_min_x && m_max_x == other.m_max_x && m_min_y == other.m_min_y &&
           m_max_y == other.m_max_y && m_min_z == other.m_min_z && m_max_z == other.m_max_z;
}

} // namespace lvox

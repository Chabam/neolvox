#include <lvox/scanner/beam.hpp>

lvox::Beam::Beam(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction)
    : m_origin{origin}
    , m_direction{direction}
{
}

lvox::Beam::Beam(Beam&& other)
    : m_origin{std::move(other.m_origin)}
    , m_direction{std::move(other.m_direction)}
{
}

lvox::Beam::Beam(const Beam& other)
    : m_origin{other.m_origin}
    , m_direction{other.m_direction}
{
}

auto lvox::Beam::operator=(Beam&& other) -> lvox::Beam&
{
    m_origin = std::move(other.m_origin);
    m_direction = std::move(other.m_direction);

    return *this;
}

auto lvox::Beam::operator=(const Beam& other) -> lvox::Beam&
{
    m_origin = other.m_origin;
    m_direction = other.m_direction;

    return *this;
}

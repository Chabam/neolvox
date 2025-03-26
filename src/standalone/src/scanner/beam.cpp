#include <lvox/scanner/beam.hpp>

lvox::scanner::Beam::Beam(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction)
    : m_origin{origin}
    , m_direction{direction}
{
}

lvox::scanner::Beam::Beam(Beam&& other)
    : m_origin{std::move(other.m_origin)}
    , m_direction{std::move(other.m_direction)}
{
}

lvox::scanner::Beam::Beam(const Beam& other)
    : m_origin{other.m_origin}
    , m_direction{other.m_direction}
{
}

auto lvox::scanner::Beam::operator=(Beam&& other) -> Beam&
{
    m_origin = std::move(other.m_origin);
    m_direction = std::move(other.m_direction);

    return *this;
}

auto lvox::scanner::Beam::operator=(const Beam& other) -> Beam&
{
    m_origin = other.m_origin;
    m_direction = other.m_direction;

    return *this;
}

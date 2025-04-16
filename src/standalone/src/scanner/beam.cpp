#include <lvox/scanner/beam.hpp>
namespace lvox
{

Beam::Beam(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction)
    : m_origin{origin}
    , m_direction{direction.normalized()}
{
}

Beam::Beam(Beam&& other)
    : m_origin{std::move(other.m_origin)}
    , m_direction{std::move(other.m_direction)}
{
}

Beam::Beam(const Beam& other)
    : m_origin{other.m_origin}
    , m_direction{other.m_direction}
{
}

auto Beam::operator=(Beam&& other) -> Beam&
{
    m_origin    = std::move(other.m_origin);
    m_direction = std::move(other.m_direction);

    return *this;
}

auto Beam::operator=(const Beam& other) -> Beam&
{
    m_origin    = other.m_origin;
    m_direction = other.m_direction;

    return *this;
}

} // namespace lvox

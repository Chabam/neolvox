#include <lvox/scanner/beam.hpp>

namespace lvox
{

Beam::Beam(const Point& origin, const Vector& direction)
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

Beam& Beam::operator=(Beam&& other)
{
    m_origin    = std::move(other.m_origin);
    m_direction = std::move(other.m_direction);

    return *this;
}

Beam& Beam::operator=(const Beam& other)
{
    m_origin    = other.m_origin;
    m_direction = other.m_direction;

    return *this;
}

} // namespace lvox

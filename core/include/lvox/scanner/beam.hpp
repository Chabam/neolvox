#ifndef LVOX_SCANNER_BEAM_HPP
#define LVOX_SCANNER_BEAM_HPP

#include <lvox/types.hpp>

namespace lvox
{

template <Point PointT>
class Beam
{
  public:
    Beam(const PointT& origin, const Vector& direction)
        : m_origin{origin}
        , m_direction{direction.normalized()}
    {
    }

    Beam(Beam&& other)
        : m_origin{std::move(other.m_origin)}
        , m_direction{std::move(other.m_direction)}
    {
    }

    Beam(const Beam& other)
        : m_origin{other.m_origin}
        , m_direction{other.m_direction}
    {
    }

    Beam& operator=(Beam&& other)
    {
        m_origin    = std::move(other.m_origin);
        m_direction = std::move(other.m_direction);

        return *this;
    }

    Beam& operator=(const Beam& other)
    {
        m_origin    = other.m_origin;
        m_direction = other.m_direction;

        return *this;
    }

    PointT origin() const { return m_origin; };
    Vector direction() const { return m_direction; };

  private:
    PointT m_origin;
    Vector m_direction;
};

} // namespace lvox

#endif // !LVOX_SCANNER_BEAM_HPP

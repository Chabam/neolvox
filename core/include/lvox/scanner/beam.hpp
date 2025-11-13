#ifndef LVOX_SCANNER_BEAM_HPP
#define LVOX_SCANNER_BEAM_HPP

#include <lvox/types.hpp>

namespace lvox
{

class Beam
{
  public:
    Beam() = default;
    Beam(const Point& origin, const Vector& direction);

    ~Beam() = default;

    Beam(Beam&&);
    Beam(const Beam&);
    Beam& operator=(Beam&&);
    Beam& operator=(const Beam&);

    Point origin() const { return m_origin; };
    Point direction() const { return m_direction; };

  private:
    Point  m_origin;
    Vector m_direction;
};

} // namespace lvox

#endif // !LVOX_SCANNER_BEAM_HPP

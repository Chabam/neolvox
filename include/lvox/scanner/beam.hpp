#ifndef LVOX_SCANNER_BEAM_HPP
#define LVOX_SCANNER_BEAM_HPP

#include <lvox/lvox_types.hpp>

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
    auto operator=(Beam&&) -> Beam&;
    auto operator=(const Beam&) -> Beam&;

    auto origin() const -> Point { return m_origin; };
    auto direction() const -> Point { return m_direction; };

  private:
    Point  m_origin;
    Vector m_direction;
};

} // namespace lvox

#endif // !LVOX_SCANNER_BEAM_HPP

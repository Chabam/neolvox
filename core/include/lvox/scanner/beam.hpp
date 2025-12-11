#ifndef LVOX_SCANNER_BEAM_HPP
#define LVOX_SCANNER_BEAM_HPP

#include <lvox/types.hpp>

namespace lvox
{

class Beam
{
  public:
    Beam(const Vector& origin, const Vector& direction);
    Beam(Beam&& other);
    Beam(const Beam& other);

    Beam& operator=(Beam&& other);
    Beam& operator=(const Beam& other);

    Vector origin() const { return m_origin; };
    Vector direction() const { return m_direction; };

  private:
    Vector m_origin;
    Vector m_direction;
};

} // namespace lvox

#endif // !LVOX_SCANNER_BEAM_HPP

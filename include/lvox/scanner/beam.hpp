#ifndef LVOX_SCANNER_BEAM_HPP
#define LVOX_SCANNER_BEAM_HPP

#include <Eigen/Eigen>

#include <pdal/Dimension.hpp>

namespace lvox
{

class Beam
{
  public:
    using vector_t = Eigen::Vector3d;

    Beam() = default;
    Beam(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction)
        : m_origin{origin}
        , m_direction{direction.normalized()}
    {
    }

    ~Beam() = default;

    Beam(Beam&&);
    Beam(const Beam&);
    auto operator=(Beam&&) -> Beam&;
    auto operator=(const Beam&) -> Beam&;

    auto origin() const -> vector_t { return m_origin; };
    auto direction() const -> vector_t { return m_direction; };

  private:
    vector_t m_origin;
    vector_t m_direction;
};

} // namespace lvox

#endif // !LVOX_SCANNER_BEAM_HPP

#ifndef LVOX_SCANNER_BEAM_HPP
#define LVOX_SCANNER_BEAM_HPP

#include <Eigen/Eigen>

#include <pdal/Dimension.hpp>

namespace lvox
{

class Beam
{
  public:
    Beam() = default;
    Beam(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction);
    ~Beam() = default;

    Beam(Beam&&);
    Beam(const Beam&);
    auto operator=(Beam&&) -> Beam&;
    auto operator=(const Beam&) -> Beam&;

  private:
    Eigen::Vector3d m_origin;
    Eigen::Vector3d m_direction;
};

} // namespace lvox

#endif // !LVOX_SCANNER_BEAM_HPP

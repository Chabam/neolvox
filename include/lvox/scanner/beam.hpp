#ifndef LVOX_SCANNER_BEAM_HPP
#define LVOX_SCANNER_BEAM_HPP

#include <Eigen/Eigen>

#include <pdal/Dimension.hpp>

namespace lvox::scanner
{
class Beam
{
  public:
    Beam() = default;
    Beam(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction);
    ~Beam() = default;

    Beam(Beam&&);
    Beam(const Beam&);
    Beam& operator=(Beam&&);
    Beam& operator=(const Beam&);

  private:
    Eigen::Vector3d m_origin;
    Eigen::Vector3d m_direction;
};

} // namespace lvox::scanner

#endif // !LVOX_SCANNER_BEAM_HPP

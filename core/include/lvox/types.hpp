#ifndef LVOX_TYPES_HPP
#define LVOX_TYPES_HPP

#include <Eigen/Eigen>
#include <vector>

namespace lvox
{
using Vector         = Eigen::Vector3d;
using Point          = Eigen::Vector3d;

struct TimedPoint
{
    double m_gps_time;
    Point m_point;
};

using PointCloud     = std::vector<TimedPoint>;
using PointCloudView = std::unique_ptr<PointCloud>;
using Index3D        = std::array<unsigned int, 3>;
} // namespace lvox

#endif // LVOX_TYPES_HPP

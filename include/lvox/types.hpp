#ifndef LVOX_TYPES_HPP
#define LVOX_TYPES_HPP

#include <Eigen/Eigen>
#include <vector>

#include <pdal/PDALUtils.hpp>

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
using Bounds         = pdal::BOX3D;
using Index          = std::size_t;
using Index3D        = std::array<unsigned int, 3>;
} // namespace lvox

#endif // LVOX_TYPES_HPP

#ifndef LVOX_TYPES_HPP
#define LVOX_TYPES_HPP

#include <Eigen/Eigen>

#include <pdal/PDALUtils.hpp>

namespace lvox
{

using Vector         = Eigen::Vector3d;
using Point          = Eigen::Vector3d;
using PointCloudView = pdal::PointViewPtr;
using Bounds         = pdal::BOX3D;
using Index          = std::size_t;
using Index3D        = std::array<Index, 3>;

} // namespace lvox

#endif // LVOX_TYPES_HPP

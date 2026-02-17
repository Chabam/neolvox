#ifndef LVOX_SCAN_HPP
#define LVOX_SCAN_HPP

#include <optional>
#include <variant>

#include <lvox/types.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/voxel/bounds.hpp>
#include <lvox/scanner/trajectory.hpp>

namespace lvox
{

template <Point PointT, PointCloud<PointT> PointCloudT>
using ScannerOrigin = std::variant<PointT, Trajectory<PointT, PointCloudT>>;

template <Point PointT, PointCloud<PointT> PointCloudT>
struct Scan
{
    const PointCloudT&                                 m_points;
    ScannerOrigin<PointT, PointCloudT>                 m_scanner_origin;
    Bounds<double>                                     m_bounds;
    std::optional<std::reference_wrapper<PointCloudT>> m_blank_shots;

    struct ComputeBeamOrigin
    {
        const double gps_time;
        Vector operator()(const PointT& scan_origin) {
            return Vector{scan_origin.x(), scan_origin.y(), scan_origin.z()};
        }

        Vector operator()(const Trajectory<PointT, PointCloudT>& trajectory)
        {
            const auto point = trajectory.interpolate_point_from_gps_time(gps_time);

            if (!point)
            {
                constexpr auto err_msg = "requested gps time for point not in trajectory {}!";
                Logger{"Compute beam origin"}.error(err_msg, gps_time);
                throw std::runtime_error(std::format(err_msg, gps_time));
            }

            return *point;
        }
    };
};

} // namespace lvox

#endif // LVOX_SCAN_HPP

#include <algorithm>
#include <filesystem>
#include <ranges>

#include <pdal/StageFactory.hpp>

#include <lvox/scanner/trajectory.hpp>

#include "lvox/types.hpp"

namespace lvox
{

// This code is greatly inspired by PDAL's georeference filter
// Trajectory class found here:
// https://github.com/PDAL/PDAL/blob/6e3d5ed48205dcef884243db6585f7da4631604c/filters/private/georeference/Trajectory.cpp
// This class can be removed if one day it becomes available from the
// cpp interface.
Trajectory::Trajectory(const std::filesystem::path& trajectory_file)
    : m_traj_points{}
{
    using dim                  = pdal::Dimension::Id;
    const std::string driver   = pdal::StageFactory::inferReaderDriver(trajectory_file.string());
    const std::string filename = trajectory_file.string();

    if (driver.empty())
        throw std::runtime_error(
            std::format("Cannot determine reader for input file: {}", filename.c_str())
        );

    pdal::PointTable pts_table;
    const auto&      layout = pts_table.layout();
    layout->registerDim(dim::GpsTime);
    layout->registerDim(dim::X);
    layout->registerDim(dim::Y);
    layout->registerDim(dim::Z);

    pdal::Options reader_opts;
    reader_opts.add("filename", filename);

    auto         factory = std::make_unique<pdal::StageFactory>();
    pdal::Stage* reader  = factory->createStage(driver);
    // world_time x y z q0 q1 q2 q3 r g b nx ny nz roll pitch yaw
    reader_opts.add(
        "header",
        "GpsTime X Y Z Q0 Q1 Q2 Q3 Red Green Blue NormalX NormalY NormalZ Pitch Roll Azimuth"
    );
    reader_opts.add("skip", 1);
    reader_opts.add("separator", " ");

    reader->setOptions(reader_opts);
    reader->prepare(pts_table);
    const auto view_set = reader->execute(pts_table);

    m_traj_points = **view_set.begin() | std::ranges::views::transform([](const auto& pt) -> Point {
        return Point{
            .m_gps_time = pt.template getFieldAs<double>(dim::GpsTime),
            .m_coord =
                {pt.template getFieldAs<double>(dim::X),
                 pt.template getFieldAs<double>(dim::Y),
                 pt.template getFieldAs<double>(dim::Z)}
        };
    }) | std::ranges::to<std::vector<Point>>(),

    std::ranges::sort(m_traj_points, [](const Point& lhs, const Point& rhs) -> bool {
        return lhs.m_gps_time < rhs.m_gps_time;
    });
}

auto Trajectory::interpolate_point_from_gps_time(double gps_time) const -> std::optional<lvox::Point>
{
    using DimId = pdal::Dimension::Id;
    auto upper  = std::lower_bound(
        m_traj_points.begin(),
        m_traj_points.end(),
        gps_time,
        [](const Point pt, double time) {
            return pt.m_gps_time < time;
        }
    );

    // gps time is not found in the trajectory
    if (upper == m_traj_points.begin() || upper == m_traj_points.end())
    {
        return {};
    }

    // Returning a point in the trajectory that is the closest to the
    // requested gps time by linear interpolation.
    const auto   p1   = *(upper - 1);
    const auto   p2   = *upper;
    const double t1   = p1.m_gps_time;
    const double t2   = p2.m_gps_time;
    const double frac = (gps_time - t1) / (t2 - t1);

    return lvox::Point{
        std::lerp(p1.m_coord.x(), p2.m_coord.x(), frac),
        std::lerp(p1.m_coord.y(), p2.m_coord.y(), frac),
        std::lerp(p1.m_coord.z(), p2.m_coord.z(), frac)
    };
}

} // namespace lvox

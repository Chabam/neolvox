#include <algorithm>
#include <filesystem>

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
    : m_pts_view{}
    , m_pts_table{}
    , m_pts_view_set{}
{
    const std::string driver   = pdal::StageFactory::inferReaderDriver(trajectory_file.string());
    const std::string filename = trajectory_file.string();
    if (driver.empty())
        throw std::runtime_error(
            std::format("Cannot determine reader for input file: {}", filename.c_str())
        );

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
    reader->prepare(m_pts_table);
    m_pts_view_set = reader->execute(m_pts_table);
    m_pts_view     = *(m_pts_view_set.begin());
}

auto Trajectory::get_point_from_gps_time(double gps_time) -> std::optional<Point>
{
    using DimId               = pdal::Dimension::Id;
    pdal::PointViewIter upper = std::lower_bound(
        m_pts_view->begin(),
        m_pts_view->end(),
        gps_time,
        [](const pdal::PointRef pt, double time) {
            return pt.getFieldAs<double>(DimId::GpsTime) < time;
        }
    );

    // gps time is not found in the trajectory
    if (upper == m_pts_view->begin() || upper == m_pts_view->end())
    {
        return {};
    }

    // Returning a point in the trajectory that is the closest to the
    // requested gps time by linear interpolation.
    pdal::PointRef p1   = *(upper - 1);
    pdal::PointRef p2   = *upper;
    const double   t1   = p1.getFieldAs<double>(DimId::GpsTime);
    const double   t2   = p2.getFieldAs<double>(DimId::GpsTime);
    const double   frac = (gps_time - t1) / (t2 - t1);

    return Point{
        .m_gps_time = std::lerp(
            p1.getFieldAs<double>(DimId::GpsTime), p2.getFieldAs<double>(DimId::GpsTime), frac
        ),
        .m_coord =
            lvox::Point{
                std::lerp(p1.getFieldAs<double>(DimId::X), p2.getFieldAs<double>(DimId::X), frac),
                std::lerp(p1.getFieldAs<double>(DimId::Y), p2.getFieldAs<double>(DimId::Y), frac),
                std::lerp(p1.getFieldAs<double>(DimId::Z), p2.getFieldAs<double>(DimId::Z), frac)
            },
        .m_orientation =
            lvox::Vector{
                std::lerp(
                    p1.getFieldAs<double>(DimId::Pitch), p2.getFieldAs<double>(DimId::Pitch), frac
                ),
                std::lerp(
                    p1.getFieldAs<double>(DimId::Roll), p2.getFieldAs<double>(DimId::Roll), frac
                ),
                std::lerp(
                    p1.getFieldAs<double>(DimId::Azimuth),
                    p2.getFieldAs<double>(DimId::Azimuth),
                    frac
                )
            }
    };
}

} // namespace lvox

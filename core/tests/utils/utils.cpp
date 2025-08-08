#include <chrono>
#include <random>
#include <ranges>
#include <utils/utils.hpp>

#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>

#include <lvox/types.hpp>
#include "lvox/voxel/bounds.hpp"

auto get_current_time_as_gps_time() -> double
{
    namespace c = std::chrono;
    const auto current_time = c::system_clock::now().time_since_epoch();
    return static_cast<double>(c::duration_cast<c::microseconds>(current_time).count());
}

Point::Point(double x, double y, double z, double gps_time)
    : x{x}
    , y{y}
    , z{z}
    , gps_time{gps_time}
{
}


auto create_bounds(double dim_x, double dim_y, double dim_z) -> lvox::Bounds
{
    const double half_dim_x = dim_x / 2.0;
    const double half_dim_y = dim_y / 2.0;
    const double half_dim_z = dim_z / 2.0;

    return lvox::Bounds{-half_dim_x, half_dim_x, -half_dim_y, half_dim_y, -half_dim_z, half_dim_z};
}

auto generate_cubic_point_cloud(pdal::PointTableRef table, double dim_x, double dim_y, double dim_z)
    -> pdal::PointViewPtr
{
    table.layout()->registerDim(pdal::Dimension::Id::X);
    table.layout()->registerDim(pdal::Dimension::Id::Y);
    table.layout()->registerDim(pdal::Dimension::Id::Z);
    table.layout()->registerDim(pdal::Dimension::Id::GpsTime);

    pdal::PointViewPtr view{std::make_unique<pdal::PointView>(table)};
    const double       half_dim_x = dim_x / 2.0;
    const double       half_dim_y = dim_y / 2.0;
    const double       half_dim_z = dim_z / 2.0;
    std::vector<Point> cube       = {
        // Top square
        Point{-half_dim_x, -half_dim_y, half_dim_z},
        Point{-half_dim_x, half_dim_y, half_dim_z},
        Point{half_dim_x, half_dim_y, half_dim_z},
        Point{half_dim_x, -half_dim_y, half_dim_z},

        // Bottom
        Point{-half_dim_x, -half_dim_y, -half_dim_z},
        Point{-half_dim_x, half_dim_y, -half_dim_z},
        Point{half_dim_x, half_dim_y, -half_dim_z},
        Point{half_dim_x, -half_dim_y, -half_dim_z},
    };

    for (size_t i = 0; i < cube.size(); ++i)
    {
        view->setField(pdal::Dimension::Id::X, i, cube[i].x);
        view->setField(pdal::Dimension::Id::Y, i, cube[i].y);
        view->setField(pdal::Dimension::Id::Z, i, cube[i].z);
    }

    return view;
}

auto generate_cubic_point_cloud_with_random_points(pdal::PointTableRef table, size_t point_count, double dim_x, double dim_y, double dim_z)
    -> pdal::PointViewPtr
{
    table.layout()->registerDim(pdal::Dimension::Id::X);
    table.layout()->registerDim(pdal::Dimension::Id::Y);
    table.layout()->registerDim(pdal::Dimension::Id::Z);
    table.layout()->registerDim(pdal::Dimension::Id::GpsTime);

    pdal::PointViewPtr view{std::make_unique<pdal::PointView>(table)};
    const double       half_dim_x = dim_x / 2.0;
    const double       half_dim_y = dim_y / 2.0;
    const double       half_dim_z = dim_z / 2.0;

    // Fixing the seed for reproducibility. Maybe not that useful?
    std::mt19937 gen{1337};
    std::uniform_real_distribution<double> dis_x(-half_dim_x, half_dim_x);
    std::uniform_real_distribution<double> dis_y(-half_dim_y, half_dim_y);
    std::uniform_real_distribution<double> dis_z(-half_dim_z, half_dim_z);

    for (lvox::Index i = 0; i < point_count; ++i)
    {
        view->setField(pdal::Dimension::Id::X, i, dis_x(gen));
        view->setField(pdal::Dimension::Id::Y, i, dis_y(gen));
        view->setField(pdal::Dimension::Id::Z, i, dis_z(gen));
    }

    return view;
}

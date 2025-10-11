#include <utils/utils.hpp>

#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>

#include <lvox/types.hpp>
#include "lvox/voxel/bounds.hpp"

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
    struct Point
    {
        double x;
        double y;
        double z;
    };

    table.layout()->registerDim(pdal::Dimension::Id::X);
    table.layout()->registerDim(pdal::Dimension::Id::Y);
    table.layout()->registerDim(pdal::Dimension::Id::Z);

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

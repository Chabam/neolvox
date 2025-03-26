#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <utils/utils.hpp>

auto create_bounds(double dim_x, double dim_y, double dim_z) -> pdal::BOX3D
{
    pdal::BOX3D bounds;
    const double half_dim_x = dim_x / 2.0;
    const double half_dim_y = dim_y / 2.0;
    const double half_dim_z = dim_z / 2.0;

    bounds.grow(half_dim_x, half_dim_y, half_dim_z);
    bounds.grow(-half_dim_x, -half_dim_y, -half_dim_z);

    return bounds;
}

auto fill_table_with_points(pdal::PointTableRef table) -> pdal::PointViewPtr
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
    std::vector<Point> cube = {
        // Top square
        Point{-1., -1., 1.},
        Point{-1., 1., 1.},
        Point{1., 1., 1.},
        Point{1., -1., 1.},

        // Bottom
        Point{-1., -1., -1.},
        Point{-1., 1., -1.},
        Point{1., 1., -1.},
        Point{1., -1., -1.},
    };

    for (size_t i = 0; i < cube.size(); ++i)
    {
        view->setField(pdal::Dimension::Id::X, i, cube[i].x);
        view->setField(pdal::Dimension::Id::Y, i, cube[i].y);
        view->setField(pdal::Dimension::Id::Z, i, cube[i].z);
    }

    return view;
}

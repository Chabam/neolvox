#include <lvox/voxel/grid.hpp>

#include <gtest/gtest.h>
#include <pdal/Dimension.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/QuickInfo.hpp>
#include <pdal/util/Bounds.hpp>

#include <utils/utils.hpp>

TEST(voxel_grid, cell_count)
{
    const double cell_size = 1.;
    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    pdal::BOX3D bounds = create_bounds(dim_x, dim_y, dim_z);

    lvox::voxel::GridU32i grid{bounds, cell_size};
    ASSERT_EQ(dim_x * dim_y * dim_z, grid.cell_count());
}

TEST(voxel_grid, at)
{
    const double cell_size = 1.;
    const size_t dim_x = 10;
    const size_t dim_y = 20;
    const size_t dim_z = 30;

    pdal::BOX3D bounds = create_bounds(dim_x, dim_y, dim_z);

    lvox::voxel::GridU32i grid{bounds, cell_size};

    for (size_t x = 0; x < dim_x; x++)
    {
        for (size_t y = 0; y < dim_y; y++)
        {
            for (size_t z = 0; z < dim_z; z++)
            {
                const size_t value_of_cell = x + y + z;
                auto at_assign = [&]() mutable {
                    grid.at(x, y, z) = value_of_cell;
                };
                ASSERT_NO_THROW(std::invoke(at_assign));
                ASSERT_EQ(value_of_cell, grid.at(x, y, z));
            }
        }
    }
}

TEST(voxel_grid, creation_from_box3d)
{
    const double dim_x = 20;
    const double dim_y = 40;
    const double dim_z = 60;

    pdal::BOX3D bounds = create_bounds(dim_x, dim_y, dim_z);

    {
        const double cell_size = 1.;
        const lvox::voxel::GridU32i grid{bounds, cell_size};
        EXPECT_EQ(dim_x, grid.dim_x());
        EXPECT_EQ(dim_y, grid.dim_y());
        EXPECT_EQ(dim_z, grid.dim_z());

        const pdal::BOX3D grid_bounds = grid.bounds();

        EXPECT_EQ(-10., grid_bounds.minx);
        EXPECT_EQ(10., grid_bounds.maxx);
        EXPECT_EQ(-20., grid_bounds.miny);
        EXPECT_EQ(20., grid_bounds.maxy);
        EXPECT_EQ(-30., grid_bounds.minz);
        EXPECT_EQ(30., grid_bounds.maxz);
    }

    {

        const double cell_size = .5;
        const lvox::voxel::GridU32i grid{bounds, cell_size};
        EXPECT_EQ(dim_x * 2, grid.dim_x());
        EXPECT_EQ(dim_y * 2, grid.dim_y());
        EXPECT_EQ(dim_z * 2, grid.dim_z());

        const pdal::BOX3D grid_bounds = grid.bounds();

        EXPECT_EQ(-10., grid_bounds.minx);
        EXPECT_EQ(10., grid_bounds.maxx);
        EXPECT_EQ(-20., grid_bounds.miny);
        EXPECT_EQ(20., grid_bounds.maxy);
        EXPECT_EQ(-30., grid_bounds.minz);
        EXPECT_EQ(30., grid_bounds.maxz);
    }

    {
        const double cell_size = 15.;
        const lvox::voxel::GridU32i grid{bounds, cell_size};
        // 20 / 15 = 1.33333 => 2
        EXPECT_EQ(2, grid.dim_x());
        // 40 / 15 = 1.33333 => 3
        EXPECT_EQ(3, grid.dim_y());
        // 60 / 15 = 4
        EXPECT_EQ(4, grid.dim_z());

        pdal::BOX3D grid_bounds = grid.bounds();

        EXPECT_EQ(-10., grid_bounds.minx);
        // -10 + 2 * 15 = 20
        EXPECT_EQ(20., grid_bounds.maxx);
        EXPECT_EQ(-20., grid_bounds.miny);
        // -20 + 3 * 15 = 20
        EXPECT_EQ(25., grid_bounds.maxy);
        EXPECT_EQ(-30., grid_bounds.minz);
        // -30 + 4 * 15 = 30
        EXPECT_EQ(30., grid_bounds.maxz);
    }
}

TEST(voxel_grid, create_from_point_cloud)
{
    pdal::PointTable table;
    const auto view = fill_table_with_points(table);

    pdal::BOX3D point_cloud_bounds;
    const double cell_size = 0.5;
    view->calculateBounds(point_cloud_bounds);
    lvox::voxel::GridU32i grid{point_cloud_bounds, cell_size};

    for (const auto pt : *view)
    {
        const double x = pt.getFieldAs<double>(pdal::Dimension::Id::X);
        const double y = pt.getFieldAs<double>(pdal::Dimension::Id::Y);
        const double z = pt.getFieldAs<double>(pdal::Dimension::Id::Z);
        ASSERT_TRUE(grid.bounds().contains(x, y, z));
    }
}

TEST(voxel_grid, voxel_bounds)
{
    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    pdal::BOX3D bounds = create_bounds(dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::voxel::GridU32i grid{bounds, cell_size};
    const pdal::BOX3D grid_bounds = grid.bounds();

    {
        const pdal::BOX3D bound = grid.voxel_bounds(0, 0, 0);
        EXPECT_EQ(-5., bound.minx);
        EXPECT_EQ(-5. + cell_size, bound.maxx);
        EXPECT_EQ(-10., bound.miny);
        EXPECT_EQ(-10. + cell_size, bound.maxy);
        EXPECT_EQ(-15., bound.minz);
        EXPECT_EQ(-15. + cell_size, bound.maxz);
    }

    {
        const pdal::BOX3D bound = grid.voxel_bounds(dim_x / 2., dim_y / 2., dim_z / 2.);
        EXPECT_EQ(0., bound.minx);
        EXPECT_EQ(0. + cell_size, bound.maxx);
        EXPECT_EQ(0., bound.miny);
        EXPECT_EQ(0. + cell_size, bound.maxy);
        EXPECT_EQ(0., bound.minz);
        EXPECT_EQ(0. + cell_size, bound.maxz);
    }

    for (size_t x = 0; x < dim_x; x++)
    {
        for (size_t y = 0; y < dim_y; y++)
        {
            for (size_t z = 0; z < dim_z; z++)
            {
                ASSERT_TRUE(grid_bounds.contains(grid.voxel_bounds(x, y, z)));
            }
        }
    }
}

#include <lvox/voxel/grid.hpp>

#include <gtest/gtest.h>
#include <pdal/util/Bounds.hpp>

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
            for (size_t z = 0; z < dim_y; z++)
            {
                const size_t value_of_cell = x + y + z;
                auto at_assign = [&]() mutable { grid.at(x, y, z) = value_of_cell; };
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

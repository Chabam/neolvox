#include <lvox/voxel/grid.hpp>

#include <gtest/gtest.h>

TEST(voxel_grid, cell_count)
{
    const double cell_size = 1.;
    const size_t dim_x = 10;
    const size_t dim_y = 20;
    const size_t dim_z = 30;
    lvox::voxel::GridU32i grid{cell_size, dim_x, dim_y, dim_z};
    ASSERT_EQ(dim_x * dim_y * dim_z, grid.cell_count());
}

TEST(voxel_grid, at)
{
    const double cell_size = 1.;
    const size_t dim_x = 10;
    const size_t dim_y = 20;
    const size_t dim_z = 30;
    lvox::voxel::GridU32i grid{cell_size, dim_x, dim_y, dim_z};

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
    pdal::BOX3D bounds;
    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    bounds.grow(dim_x, dim_y, dim_z);
    bounds.grow(-dim_x, -dim_y, -dim_z);

    {
        lvox::voxel::GridU32i grid{bounds, 1.};
        EXPECT_EQ(static_cast<size_t>(dim_x * 2), grid.dim_x());
        EXPECT_EQ(static_cast<size_t>(dim_y * 2), grid.dim_y());
        EXPECT_EQ(static_cast<size_t>(dim_z * 2), grid.dim_z());
    }

    {
        lvox::voxel::GridU32i grid{bounds, .5};
        EXPECT_EQ(static_cast<size_t>(dim_x * 4), grid.dim_x());
        EXPECT_EQ(static_cast<size_t>(dim_y * 4), grid.dim_y());
        EXPECT_EQ(static_cast<size_t>(dim_z * 4), grid.dim_z());
    }

    {
        lvox::voxel::GridU32i grid{bounds, 15.};
        // 20 / 15 = 1.33333 => 2
        EXPECT_EQ(2, grid.dim_x());
        // 40 / 15 = 1.33333 => 3
        EXPECT_EQ(3, grid.dim_y());
        // 60 / 15 = 4
        EXPECT_EQ(4, grid.dim_z());
    }
}

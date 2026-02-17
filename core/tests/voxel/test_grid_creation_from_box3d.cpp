#include <gtest/gtest.h>
#include <utils/utils.hpp>

#include <lvox/voxel/grid.hpp>

TEST(grid, creation_from_box3d)
{
    const double dim_x = 20;
    const double dim_y = 40;
    const double dim_z = 60;

    lvox::Bounds bounds = create_bounds(dim_x, dim_y, dim_z);

    {
        const double            cell_size = 1.;
        const lvox::BoundedGrid grid{bounds, cell_size};
        EXPECT_EQ(dim_x, grid.dim_x());
        EXPECT_EQ(dim_y, grid.dim_y());
        EXPECT_EQ(dim_z, grid.dim_z());

        const lvox::Bounds grid_bounds = grid.bounds();

        EXPECT_EQ(-10., grid_bounds.m_min_x);
        EXPECT_EQ(10., grid_bounds.m_max_x);
        EXPECT_EQ(-20., grid_bounds.m_min_y);
        EXPECT_EQ(20., grid_bounds.m_max_y);
        EXPECT_EQ(-30., grid_bounds.m_min_z);
        EXPECT_EQ(30., grid_bounds.m_max_z);
    }

    {

        const double            cell_size = .5;
        const lvox::BoundedGrid grid{bounds, cell_size};
        EXPECT_EQ(dim_x * 2, grid.dim_x());
        EXPECT_EQ(dim_y * 2, grid.dim_y());
        EXPECT_EQ(dim_z * 2, grid.dim_z());

        const lvox::Bounds grid_bounds = grid.bounds();

        EXPECT_EQ(-10., grid_bounds.m_min_x);
        EXPECT_EQ(10., grid_bounds.m_max_x);
        EXPECT_EQ(-20., grid_bounds.m_min_y);
        EXPECT_EQ(20., grid_bounds.m_max_y);
        EXPECT_EQ(-30., grid_bounds.m_min_z);
        EXPECT_EQ(30., grid_bounds.m_max_z);
    }

    {
        const double            cell_size = 15.;
        const lvox::BoundedGrid grid{bounds, cell_size};
        // 10 / 15 => 0.66667 => 1 * 15 => 15
        // (10 - (-10)) / 15 = 1.33333 => 2
        EXPECT_EQ(2, grid.dim_x());
        // 20 / 15 => 1.3333 => 2 => 2 * 15 => 30
        // (30 - (-30)) / 15 = 4
        EXPECT_EQ(4, grid.dim_y());
        // 30 / 15 => 2 => 2 * 15 => 30
        // 60 / 15 = 4
        EXPECT_EQ(4, grid.dim_z());

        lvox::Bounds grid_bounds = grid.bounds();

        EXPECT_EQ(-15., grid_bounds.m_min_x);
        EXPECT_EQ(15., grid_bounds.m_max_x);
        EXPECT_EQ(-30., grid_bounds.m_min_y);
        EXPECT_EQ(30., grid_bounds.m_max_y);
        EXPECT_EQ(-30., grid_bounds.m_min_z);
        EXPECT_EQ(30., grid_bounds.m_max_z);
    }
}

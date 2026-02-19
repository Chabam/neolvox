#include <gtest/gtest.h>
#include <utils/utils.hpp>

#include <lvox/voxel/grid.hpp>

TEST(grid, creation_from_bounds)
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

TEST(grid, creation_from_bounds_with_alignment)
{
    const double dim_x = 20;
    const double dim_y = 40;
    const double dim_z = 60;

    lvox::Bounds bounds = create_bounds(dim_x, dim_y, dim_z);

    {
        const double            cell_size = 1.;
        const lvox::BoundedGrid grid{bounds, cell_size, 8};
        EXPECT_EQ(32, grid.dim_x());
        EXPECT_EQ(48, grid.dim_y());
        EXPECT_EQ(64, grid.dim_z());

        lvox::Bounds grid_bounds = grid.bounds();

        EXPECT_EQ(-16., grid_bounds.m_min_x);
        EXPECT_EQ(16., grid_bounds.m_max_x);
        EXPECT_EQ(-24., grid_bounds.m_min_y);
        EXPECT_EQ(24., grid_bounds.m_max_y);
        EXPECT_EQ(-32., grid_bounds.m_min_z);
        EXPECT_EQ(32., grid_bounds.m_max_z);
    }

    {
        const double            cell_size = 1.;
        const lvox::BoundedGrid grid{bounds, cell_size};
        const lvox::BoundedGrid grid_aligned{bounds, cell_size, 8};

        EXPECT_GT(grid_aligned.dim_x(), grid.dim_x());
        EXPECT_GT(grid_aligned.dim_y(), grid.dim_y());
        EXPECT_GT(grid_aligned.dim_z(), grid.dim_z());

        lvox::Bounds grid_bounds         = grid.bounds();
        lvox::Bounds aligned_grid_bounds = grid_aligned.bounds();

        EXPECT_LT(aligned_grid_bounds.m_min_x, grid_bounds.m_min_x);
        EXPECT_GT(aligned_grid_bounds.m_max_x, grid_bounds.m_max_x);
        EXPECT_LT(aligned_grid_bounds.m_min_y, grid_bounds.m_min_y);
        EXPECT_GT(aligned_grid_bounds.m_max_y, grid_bounds.m_max_y);
        EXPECT_LT(aligned_grid_bounds.m_min_z, grid_bounds.m_min_z);
        EXPECT_GT(aligned_grid_bounds.m_max_z, grid_bounds.m_max_z);
    }

    {
        lvox::Bounds<double>    negative_bounds{-20, -10, -20, -10, -20, -10};
        const double            cell_size = 1.;
        const lvox::BoundedGrid grid{negative_bounds, cell_size};
        const lvox::BoundedGrid grid_aligned{negative_bounds, cell_size, 8};

        EXPECT_GT(grid_aligned.dim_x(), grid.dim_x());
        EXPECT_GT(grid_aligned.dim_y(), grid.dim_y());
        EXPECT_GT(grid_aligned.dim_z(), grid.dim_z());

        lvox::Bounds grid_bounds         = grid.bounds();
        lvox::Bounds aligned_grid_bounds = grid_aligned.bounds();

        EXPECT_LT(aligned_grid_bounds.m_min_x, grid_bounds.m_min_x);
        EXPECT_GT(aligned_grid_bounds.m_max_x, grid_bounds.m_max_x);
        EXPECT_LT(aligned_grid_bounds.m_min_y, grid_bounds.m_min_y);
        EXPECT_GT(aligned_grid_bounds.m_max_y, grid_bounds.m_max_y);
        EXPECT_LT(aligned_grid_bounds.m_min_z, grid_bounds.m_min_z);
        EXPECT_GT(aligned_grid_bounds.m_max_z, grid_bounds.m_max_z);
    }

    {
        lvox::Bounds<double>    negative_bounds{10, 20, 10, 20, 10, 20};
        const double            cell_size = 1.;
        const lvox::BoundedGrid grid{negative_bounds, cell_size};
        const lvox::BoundedGrid grid_aligned{negative_bounds, cell_size, 8};

        EXPECT_GT(grid_aligned.dim_x(), grid.dim_x());
        EXPECT_GT(grid_aligned.dim_y(), grid.dim_y());
        EXPECT_GT(grid_aligned.dim_z(), grid.dim_z());

        lvox::Bounds grid_bounds         = grid.bounds();
        lvox::Bounds aligned_grid_bounds = grid_aligned.bounds();

        EXPECT_LT(aligned_grid_bounds.m_min_x, grid_bounds.m_min_x);
        EXPECT_GT(aligned_grid_bounds.m_max_x, grid_bounds.m_max_x);
        EXPECT_LT(aligned_grid_bounds.m_min_y, grid_bounds.m_min_y);
        EXPECT_GT(aligned_grid_bounds.m_max_y, grid_bounds.m_max_y);
        EXPECT_LT(aligned_grid_bounds.m_min_z, grid_bounds.m_min_z);
        EXPECT_GT(aligned_grid_bounds.m_max_z, grid_bounds.m_max_z);
    }
}

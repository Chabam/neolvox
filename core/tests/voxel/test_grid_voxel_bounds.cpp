#include <gtest/gtest.h>
#include <utils/utils.hpp>
#include <lvox/voxel/grid.hpp>

TEST(grid, voxel_bounds)
{
    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    lvox::Bounds bounds = create_bounds(dim_x, dim_y, dim_z);

    const double       cell_size = 1.;
    lvox::BoundedGrid  grid{bounds, cell_size};
    const lvox::Bounds grid_bounds  = grid.bounds();
    const lvox::Bounds index_bounds = grid.index_bounds();

    {
        const lvox::Bounds bound =
            grid.voxel_bounds(index_bounds.m_min_x, index_bounds.m_min_y, index_bounds.m_min_z);
        EXPECT_EQ(-5., bound.m_min_x);
        EXPECT_EQ(-5. + cell_size, bound.m_max_x);
        EXPECT_EQ(-10., bound.m_min_y);
        EXPECT_EQ(-10. + cell_size, bound.m_max_y);
        EXPECT_EQ(-15., bound.m_min_z);
        EXPECT_EQ(-15. + cell_size, bound.m_max_z);
    }

    {
        const lvox::Bounds bound = grid.voxel_bounds(0, 0, 0);
        EXPECT_EQ(0., bound.m_min_x);
        EXPECT_EQ(0. + cell_size, bound.m_max_x);
        EXPECT_EQ(0., bound.m_min_y);
        EXPECT_EQ(0. + cell_size, bound.m_max_y);
        EXPECT_EQ(0., bound.m_min_z);
        EXPECT_EQ(0. + cell_size, bound.m_max_z);
    }

    for (size_t x = index_bounds.m_min_x; x < index_bounds.m_max_x; x++)
    {
        for (size_t y = index_bounds.m_min_y; y < index_bounds.m_max_y; y++)
        {
            for (size_t z = index_bounds.m_min_z; z < index_bounds.m_max_z; z++)
            {
                ASSERT_TRUE(grid_bounds.contains(grid.voxel_bounds(x, y, z)));
            }
        }
    }
}

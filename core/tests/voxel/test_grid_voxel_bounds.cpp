#include <gtest/gtest.h>
#include <utils/utils.hpp>
#include <lvox/voxel/grid.hpp>

TEST(grid, voxel_bounds)
{
    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    lvox::Bounds bounds = create_bounds(dim_x, dim_y, dim_z);

    const double                 cell_size = 1.;
    lvox::BoundedGrid grid{bounds, cell_size};
    const lvox::Bounds   grid_bounds = grid.bounds();

    {
        const lvox::Bounds bound = grid.voxel_bounds(0, 0, 0);
        EXPECT_EQ(-5., bound.m_min_x);
        EXPECT_EQ(-5. + cell_size, bound.m_max_x);
        EXPECT_EQ(-10., bound.m_min_y);
        EXPECT_EQ(-10. + cell_size, bound.m_max_y);
        EXPECT_EQ(-15., bound.m_min_z);
        EXPECT_EQ(-15. + cell_size, bound.m_max_z);
    }

    {
        const lvox::Bounds bound = grid.voxel_bounds(dim_x / 2., dim_y / 2., dim_z / 2.);
        EXPECT_EQ(0., bound.m_min_x);
        EXPECT_EQ(0. + cell_size, bound.m_max_x);
        EXPECT_EQ(0., bound.m_min_y);
        EXPECT_EQ(0. + cell_size, bound.m_max_y);
        EXPECT_EQ(0., bound.m_min_z);
        EXPECT_EQ(0. + cell_size, bound.m_max_z);
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

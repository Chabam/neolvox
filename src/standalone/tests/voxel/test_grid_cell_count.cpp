#include <gtest/gtest.h>
#include <utils/utils.hpp>
#include <lvox/voxel/grid.hpp>

TEST(grid, cell_count)
{
    const double cell_size = 1.;
    const double dim_x     = 8;
    const double dim_y     = 8;
    const double dim_z     = 8;

    lvox::Bounds bounds = create_bounds(dim_x, dim_y, dim_z);

    lvox::BoundedGrid grid{bounds, cell_size};
    ASSERT_EQ(dim_x * dim_y * dim_z, grid.cell_count());
}

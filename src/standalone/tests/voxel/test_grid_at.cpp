#include <gtest/gtest.h>
#include <utils/utils.hpp>
#include <lvox/voxel/grid.hpp>

TEST(grid, at)
{
    const double      cell_size = 1.;
    const size_t dim_x     = 10;
    const size_t dim_y     = 20;
    const size_t dim_z     = 30;

    lvox::Bounds bounds = create_bounds(dim_x, dim_y, dim_z);

    lvox::GridU32 grid{bounds, cell_size};

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

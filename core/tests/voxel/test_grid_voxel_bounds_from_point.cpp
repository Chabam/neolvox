#include <gtest/gtest.h>
#include <utils/utils.hpp>

#include <lvox/voxel/grid.hpp>

TEST(grid, voxel_bounds_from_point)
{
    // Very concious choice of an odd number
    const double dim = 9;

    lvox::Bounds bounds = create_bounds(dim, dim, dim);

    const double      cell_size = 1.;
    lvox::BoundedGrid grid{bounds, cell_size};
    auto              index_bounds = grid.index_bounds();

    const double       middle_index = index_bounds.m_min_x + (dim / 2.0);
    const lvox::Bounds middle_voxel_from_dim =
        grid.voxel_bounds(middle_index, middle_index, middle_index);
    const lvox::Bounds middle_voxel_from_point =
        grid.voxel_bounds_from_point(lvox::Vector::Constant(0.));

    EXPECT_EQ(middle_voxel_from_dim, middle_voxel_from_point);
}

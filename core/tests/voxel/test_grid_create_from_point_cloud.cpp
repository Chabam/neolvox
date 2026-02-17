#include <gtest/gtest.h>
#include <utils/utils.hpp>
#include <lvox/voxel/grid.hpp>

TEST(grid, create_from_point_cloud)
{
    const auto pc = generate_cubic_point_cloud();

    lvox::Bounds<double> point_cloud_bounds;
    const double         cell_size = 0.5;

    for (const auto& pt : pc)
    {
        point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());
    }
    lvox::BoundedGrid grid{point_cloud_bounds, cell_size};

    for (const auto pt : pc)
    {
        ASSERT_TRUE(grid.bounds().contains(pt.x(), pt.y(), pt.z()));
    }
}

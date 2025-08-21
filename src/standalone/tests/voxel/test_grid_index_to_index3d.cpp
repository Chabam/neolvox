#include <gtest/gtest.h>
#include <utils/utils.hpp>
#include <lvox/voxel/grid.hpp>
#include "lvox/types.hpp"
#include <pdal/PointView.hpp>

TEST(grid, index_to_index3d)
{
    pdal::PointTable table;
    const size_t     dim_x = 10;
    const size_t     dim_y = 20;
    const size_t     dim_z = 30;
    const double     voxel_size = 1.;

    const auto bounds = create_bounds(dim_x, dim_y, dim_z);

    lvox::GridU32 grid{bounds, voxel_size};

    std::set<size_t3D> idxs;
    for (size_t i = 0; i < grid.cell_count(); ++i)
    {
        const size_t3D idx3d = grid.index_to_index3d(i);
        const auto [x, y, z] = idx3d;
        ASSERT_GE(dim_x, x);
        ASSERT_GE(dim_y, y);
        ASSERT_GE(dim_z, z);
        std::cout << std::format("{},{},{}", x, y, z) << std::endl;
        auto insert_res = idxs.insert(idx3d);

        // ASSERT_TRUE(insert_res.second);
    }

    ASSERT_EQ(grid.cell_count(), idxs.size());
}

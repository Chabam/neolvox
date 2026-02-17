#include <gtest/gtest.h>
#include <utils/utils.hpp>
#include <lvox/voxel/grid.hpp>
#include <lvox/types.hpp>

TEST(grid, index_of_point)
{
    const double     dim_x = 4;
    const double     dim_y = 4;
    const double     dim_z = 4;

    const auto pc = generate_cubic_point_cloud();

    lvox::Bounds<double> point_cloud_bounds;
    const double cell_size = .5;
    for (const auto pt : pc)
    {
        point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());
    }

    lvox::BoundedGrid grid{point_cloud_bounds, cell_size};

    std::set<lvox::Index3D> seen_idxs;
    const Eigen::Vector3d   vec = Eigen::Vector3d::Constant(-0.5);
    for (size_t x = 0; x < dim_x; x++)
    {
        for (size_t y = 0; y < dim_y; y++)
        {
            for (size_t z = 0; z < dim_z; z++)
            {
                Eigen::Vector3d offset{x * cell_size, y * cell_size, z * cell_size};
                const auto      idxs = grid.index3d_of_point(vec + offset);
                ASSERT_TRUE(!seen_idxs.contains(idxs));
            }
        }
    }

    constexpr double invalid_coord = std::numeric_limits<double>::max();
    ASSERT_ANY_THROW((void)grid.index3d_of_point(Eigen::Vector3d::Constant(invalid_coord)));
}

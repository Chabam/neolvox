#include <gtest/gtest.h>
#include <utils/utils.hpp>
#include <lvox/voxel/grid.hpp>
#include <pdal/PointView.hpp>

TEST(grid, index_of_point)
{
    pdal::PointTable table;
    const double     dim_x = 4;
    const double     dim_y = 4;
    const double     dim_z = 4;

    const auto view = generate_cubic_point_cloud(table);

    lvox::Bounds point_cloud_bounds;
    const double         cell_size = .5;
    view->calculateBounds(point_cloud_bounds);
    lvox::GridU32 grid{point_cloud_bounds, cell_size};

    std::set<std::array<size_t, 3>> seen_idxs;
    const Eigen::Vector3d           vec = Eigen::Vector3d::Constant(-0.5);
    for (size_t x = 0; x < dim_x; x++)
    {
        for (size_t y = 0; y < dim_y; y++)
        {
            for (size_t z = 0; z < dim_z; z++)
            {
                Eigen::Vector3d offset{x * cell_size, y * cell_size, z * cell_size};
                const auto      idxs = grid.index_of_point(vec + offset);
                ASSERT_TRUE(!seen_idxs.contains(idxs));
            }
        }
    }

    constexpr double invalid_coord = std::numeric_limits<double>::max();
    ASSERT_ANY_THROW((void)grid.index_of_point(Eigen::Vector3d::Constant(invalid_coord)));
}

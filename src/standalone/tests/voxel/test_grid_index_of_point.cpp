#include <voxel/test_grid_fixtures.hpp>
#include <pdal/PointView.hpp>

TYPED_TEST(VoxelGridTests, index_of_point)
{
    pdal::PointTable table;
    const double     dim_x = 4;
    const double     dim_y = 4;
    const double     dim_z = 4;

    const auto view = generate_cubic_point_cloud(table);

    lvox::Bounds point_cloud_bounds;
    const double         cell_size = .5;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};

    std::set<std::array<lvox::Index, 3>> seen_idxs;
    const Eigen::Vector3d           vec = Eigen::Vector3d::Constant(-0.5);
    for (lvox::Index x = 0; x < dim_x; x++)
    {
        for (lvox::Index y = 0; y < dim_y; y++)
        {
            for (lvox::Index z = 0; z < dim_z; z++)
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

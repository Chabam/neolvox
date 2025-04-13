#include <voxel/test_grid_fixtures.hpp>

#include <pdal/PointView.hpp>

TYPED_TEST(VoxelGridTests, grid_traversal_straight_axis)
{

    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    pdal::PointTable table;
    auto             view = generate_cubic_point_cloud(table, dim_x, dim_y, dim_z);

    const double         cell_size = 1.;
    lvox::Grid::bounds_t point_cloud_bounds;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};

    {
        lvox::Beam::vector_t pos{
            point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz
        };
        lvox::Beam::vector_t dir{1., 0., 0.};
        lvox::Beam           beam{pos, dir};

        const auto visited_voxel_idxs = lvox::Grid::traversal(grid, beam);
        ASSERT_EQ(dim_x + 1, visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(i, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(0, z);
        }
    }
    {
        lvox::Beam::vector_t pos{
            point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz
        };
        lvox::Beam::vector_t dir{0., 1., 0.};
        lvox::Beam           beam{pos, dir};

        const auto visited_voxel_idxs = lvox::Grid::traversal(grid, beam);
        ASSERT_EQ(dim_y + 1, visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(i, y);
            ASSERT_EQ(0, z);
        }
    }
    {
        lvox::Beam::vector_t pos{
            point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz
        };
        lvox::Beam::vector_t dir{0., 0., 1.};
        lvox::Beam           beam{pos, dir};

        const auto visited_voxel_idxs = lvox::Grid::traversal(grid, beam);
        ASSERT_EQ(dim_z + 1, visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(i, z);
        }
    }
}

TYPED_TEST(VoxelGridTests, grid_traversal_diagonals)
{

    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    pdal::PointTable table;
    auto             view = generate_cubic_point_cloud(table, dim_x, dim_y, dim_z);

    const double         cell_size = 1.;
    lvox::Grid::bounds_t point_cloud_bounds;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};
    {
        lvox::Beam::vector_t min{
            point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz
        };
        lvox::Beam::vector_t max{
            point_cloud_bounds.maxx, point_cloud_bounds.maxy, point_cloud_bounds.maxz
        };
        lvox::Beam::vector_t dir{max - min};
        lvox::Beam           beam{min, dir};

        const auto visited_voxel_idxs = lvox::Grid::traversal(grid, beam);
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];

        }
    }
}

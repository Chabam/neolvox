#include <voxel/test_grid_fixtures.hpp>

#include <pdal/PointView.hpp>

#include <lvox/scanner/beam.hpp>
#include <lvox/algorithms/grid_traversal.hpp>

TYPED_TEST(VoxelGridTests, grid_traversal_x_axis)
{

    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    pdal::PointTable table;
    auto             view = generate_cubic_point_cloud(table, dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};

    {
        lvox::Point  pos{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            }
        );
        ASSERT_EQ(grid.dim_x(), visited_voxel_idxs.size());
        for (lvox::Index i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(i, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(0, z);
        }
    }
    {
        lvox::Point  pos{point_cloud_bounds.maxx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Vector dir{-1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            }
        );
        ASSERT_EQ(grid.dim_x(), visited_voxel_idxs.size());
        for (lvox::Index i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(grid.dim_x() - i - 1, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(0, z);
        }
    }
}

TYPED_TEST(VoxelGridTests, grid_traversal_y_axis)
{

    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    pdal::PointTable table;
    auto             view = generate_cubic_point_cloud(table, dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};

    {
        lvox::Point  pos{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Vector dir{0., 1., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            }
        );
        ASSERT_EQ(grid.dim_y(), visited_voxel_idxs.size());
        for (lvox::Index i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(i, y);
            ASSERT_EQ(0, z);
        }
    }
    {
        lvox::Point  pos{point_cloud_bounds.minx, point_cloud_bounds.maxy, point_cloud_bounds.minz};
        lvox::Vector dir{0., -1., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            }
        );
        ASSERT_EQ(grid.dim_y(), visited_voxel_idxs.size());
        for (lvox::Index i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(grid.dim_y() - i - 1, y);
            ASSERT_EQ(0, z);
        }
    }
}

TYPED_TEST(VoxelGridTests, grid_traversal_z_axis)
{

    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    pdal::PointTable table;
    auto             view = generate_cubic_point_cloud(table, dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};
    {
        lvox::Point  pos{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Vector dir{0., 0., 1.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            }
        );
        ASSERT_EQ(grid.dim_z(), visited_voxel_idxs.size());
        for (lvox::Index i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(i, z);
        }
    }
    {
        lvox::Point  pos{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.maxz};
        lvox::Vector dir{0., 0., -1.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            }
        );
        ASSERT_EQ(grid.dim_z(), visited_voxel_idxs.size());
        for (lvox::Index i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(grid.dim_z() - i - 1, z);
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

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};

    // Since the line goes from the highest point to the lowest in diagonal, the ray should hit
    // every x, y, z levels.
    {
        lvox::Point  min{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Point  max{point_cloud_bounds.maxx, point_cloud_bounds.maxy, point_cloud_bounds.maxz};
        lvox::Vector dir{max - min};
        lvox::Beam   beam{min, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            }
        );
        // NOTE: Greater or equal because of floating point errors
        ASSERT_GE(visited_voxel_idxs.size(), std::round((max - min).norm()));

        for (lvox::Index x = 0; x < dim_x; ++x)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [x](const lvox::Index3D idx) {
                            return idx[0] == x;
                        }) != visited_voxel_idxs.end());
        }

        for (lvox::Index y = 0; y < dim_y; ++y)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [y](const lvox::Index3D idx) {
                            return idx[1] == y;
                        }) != visited_voxel_idxs.end());
        }

        for (lvox::Index z = 0; z < dim_z; ++z)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [z](const lvox::Index3D idx) {
                            return idx[2] == z;
                        }) != visited_voxel_idxs.end());
        }
    }
    // In the opposite direction as well.
    {
        lvox::Vector min{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Vector max{point_cloud_bounds.maxx, point_cloud_bounds.maxy, point_cloud_bounds.maxz};
        lvox::Vector dir{min - max};
        lvox::Beam   beam{max, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            }
        );

        // NOTE: Greater or equal because of floating point errors
        ASSERT_GE(visited_voxel_idxs.size(), std::round((max - min).norm()));
        for (lvox::Index x = 0; x < dim_x; ++x)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [x](const lvox::Index3D idx) {
                            return idx[0] == x;
                        }) != visited_voxel_idxs.end());
        }

        for (lvox::Index y = 0; y < dim_y; ++y)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [y](const lvox::Index3D idx) {
                            return idx[1] == y;
                        }) != visited_voxel_idxs.end());
        }

        for (lvox::Index z = 0; z < dim_z; ++z)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [z](const lvox::Index3D idx) {
                            return idx[2] == z;
                        }) != visited_voxel_idxs.end());
        }
    }
}

TYPED_TEST(VoxelGridTests, grid_traversal_max_distance)
{

    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    pdal::PointTable table;
    auto             view = generate_cubic_point_cloud(table, dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};

    {
        lvox::Point  pos{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            },
            (dim_x / 2) - 0.1
        );
        ASSERT_EQ(grid.dim_x() / 2, visited_voxel_idxs.size());
        for (lvox::Index i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(i, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(0, z);
        }
    }
}
TYPED_TEST(VoxelGridTests, grid_traversal_distance_in_voxel)
{
    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    pdal::PointTable table;
    auto             view = generate_cubic_point_cloud(table, dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};

    {
        lvox::Point  pos{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<double> visited_voxel_distances;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_distances](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_distances.push_back(hit.m_distance_in_voxel);
            },
            1.5
        );
        ASSERT_EQ(2, visited_voxel_distances.size());
        EXPECT_EQ(1, visited_voxel_distances[0]);
        EXPECT_EQ(0.5, visited_voxel_distances[1]);
    }

    {
        lvox::Point  pos{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<double> visited_voxel_distances;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_distances](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_distances.push_back(hit.m_distance_in_voxel);
            },
            1.
        );
        ASSERT_EQ(2, visited_voxel_distances.size());
        EXPECT_EQ(1, visited_voxel_distances[0]);
        EXPECT_EQ(0, visited_voxel_distances[1]);
    }

    {
        lvox::Point  pos{point_cloud_bounds.minx, point_cloud_bounds.miny, point_cloud_bounds.minz};
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<double> visited_voxel_distances;
        lvox::algorithms::grid_traversal(
            grid,
            beam,
            [&visited_voxel_distances](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_distances.push_back(hit.m_distance_in_voxel);
            },
            0.5
        );
        ASSERT_EQ(1, visited_voxel_distances.size());
        EXPECT_EQ(0.5, visited_voxel_distances[0]);
    }
}

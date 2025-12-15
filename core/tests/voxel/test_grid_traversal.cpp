#include <gtest/gtest.h>
#include <utils/utils.hpp>

#include <lvox/algorithms/trace_beam.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/voxel/grid.hpp>
#include <lvox/types.hpp>

TEST(grid, grid_traversal_x_axis)
{

    const double dim_x = 8;
    const double dim_y = 24;
    const double dim_z = 32;

    auto pc = generate_cubic_point_cloud(dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    for (const auto pt : pc)
    {
        point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());
    }

    lvox::BoundedGrid grid{point_cloud_bounds, cell_size};

    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::TraceBeamExactDistance{
            grid
        }(beam, [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
            visited_voxel_idxs.push_back(hit.m_index);
        });
        ASSERT_EQ(grid.dim_x(), visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(i, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(0, z);
        }
    }
    {
        lvox::Vector pos{
            point_cloud_bounds.m_max_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{-1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::TraceBeamExactDistance{
            grid
        }(beam, [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
            visited_voxel_idxs.push_back(hit.m_index);
        });
        ASSERT_EQ(grid.dim_x(), visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(grid.dim_x() - i - 1, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(0, z);
        }
    }
}

TEST(grid, grid_traversal_y_axis)
{

    const double dim_x = 8;
    const double dim_y = 24;
    const double dim_z = 32;

    auto pc = generate_cubic_point_cloud(dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    for (const auto pt : pc)
    {
        point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());
    }

    lvox::BoundedGrid grid{point_cloud_bounds, cell_size};

    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{0., 1., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::TraceBeamExactDistance{
            grid
        }(beam, [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
            visited_voxel_idxs.push_back(hit.m_index);
        });
        ASSERT_EQ(grid.dim_y(), visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(i, y);
            ASSERT_EQ(0, z);
        }
    }
    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_max_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{0., -1., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::TraceBeamExactDistance{
            grid
        }(beam, [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
            visited_voxel_idxs.push_back(hit.m_index);
        });
        ASSERT_EQ(grid.dim_y(), visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(grid.dim_y() - i - 1, y);
            ASSERT_EQ(0, z);
        }
    }
}

TEST(grid, grid_traversal_z_axis)
{

    const double dim_x = 8;
    const double dim_y = 24;
    const double dim_z = 32;

    auto pc = generate_cubic_point_cloud(dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    for (const auto pt : pc)
    {
        point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());
    }

    lvox::BoundedGrid grid{point_cloud_bounds, cell_size};
    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{0., 0., 1.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::TraceBeamExactDistance{
            grid
        }(beam, [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
            visited_voxel_idxs.push_back(hit.m_index);
        });
        ASSERT_EQ(grid.dim_z(), visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(i, z);
        }
    }
    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_max_z
        };
        lvox::Vector dir{0., 0., -1.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::TraceBeamExactDistance{
            grid
        }(beam, [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
            visited_voxel_idxs.push_back(hit.m_index);
        });
        ASSERT_EQ(grid.dim_z(), visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(0, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(grid.dim_z() - i - 1, z);
        }
    }
}

TEST(grid, grid_traversal_diagonals)
{

    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    auto pc = generate_cubic_point_cloud(dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    for (const auto pt : pc)
    {
        point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());
    }

    lvox::BoundedGrid grid{point_cloud_bounds, cell_size};

    // Since the line goes from the highest point to the lowest in diagonal, the ray should hit
    // every x, y, z levels.
    {
        lvox::Vector min{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector max{
            point_cloud_bounds.m_max_x, point_cloud_bounds.m_max_y, point_cloud_bounds.m_max_z
        };
        lvox::Vector dir{max - min};
        lvox::Beam   beam{min, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::TraceBeamExactDistance{
            grid
        }(beam, [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
            visited_voxel_idxs.push_back(hit.m_index);
        });
        // NOTE: Greater or equal because of floating point errors
        ASSERT_GE(visited_voxel_idxs.size(), std::round((max - min).norm()));

        for (size_t x = 0; x < dim_x; ++x)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [x](const lvox::Index3D idx) {
                            return idx[0] == x;
                        }) != visited_voxel_idxs.end());
        }

        for (size_t y = 0; y < dim_y; ++y)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [y](const lvox::Index3D idx) {
                            return idx[1] == y;
                        }) != visited_voxel_idxs.end());
        }

        for (size_t z = 0; z < dim_z; ++z)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [z](const lvox::Index3D idx) {
                            return idx[2] == z;
                        }) != visited_voxel_idxs.end());
        }
    }
    // In the opposite direction as well.
    {
        lvox::Vector min{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector max{
            point_cloud_bounds.m_max_x, point_cloud_bounds.m_max_y, point_cloud_bounds.m_max_z
        };
        lvox::Vector dir{min - max};
        lvox::Beam   beam{max, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::TraceBeamExactDistance{
            grid
        }(beam, [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
            visited_voxel_idxs.push_back(hit.m_index);
        });

        // NOTE: Greater or equal because of floating point errors
        ASSERT_GE(visited_voxel_idxs.size(), std::round((max - min).norm()));
        for (size_t x = 0; x < dim_x; ++x)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [x](const lvox::Index3D idx) {
                            return idx[0] == x;
                        }) != visited_voxel_idxs.end());
        }

        for (size_t y = 0; y < dim_y; ++y)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [y](const lvox::Index3D idx) {
                            return idx[1] == y;
                        }) != visited_voxel_idxs.end());
        }

        for (size_t z = 0; z < dim_z; ++z)
        {
            ASSERT_TRUE(std::ranges::find_if(visited_voxel_idxs, [z](const lvox::Index3D idx) {
                            return idx[2] == z;
                        }) != visited_voxel_idxs.end());
        }
    }
}

TEST(grid, grid_traversal_max_distance)
{

    const double dim_x = 8;
    const double dim_y = 24;
    const double dim_z = 32;

    auto             pc = generate_cubic_point_cloud(dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    for (const auto pt : pc)
    {
        point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());
    }

    lvox::BoundedGrid grid{point_cloud_bounds, cell_size};

    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<lvox::Index3D> visited_voxel_idxs;
        lvox::algorithms::TraceBeamExactDistance{grid}(
            beam,
            [&visited_voxel_idxs](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_idxs.push_back(hit.m_index);
            },
            (dim_x / 2) - 0.1
        );
        ASSERT_EQ(grid.dim_x() / 2, visited_voxel_idxs.size());
        for (size_t i = 0; i < visited_voxel_idxs.size(); ++i)
        {
            auto [x, y, z] = visited_voxel_idxs[i];
            ASSERT_EQ(i, x);
            ASSERT_EQ(0, y);
            ASSERT_EQ(0, z);
        }
    }
}

TEST(grid, grid_traversal_exact_distance_in_voxel)
{
    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    auto             pc = generate_cubic_point_cloud(dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    for (const auto pt : pc)
    {
        point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());
    }

    lvox::BoundedGrid grid{point_cloud_bounds, cell_size};

    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<double> visited_voxel_distances;
        lvox::algorithms::TraceBeamExactDistance{grid}(
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
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<double> visited_voxel_distances;
        lvox::algorithms::TraceBeamExactDistance{grid}(
            beam,
            [&visited_voxel_distances](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_distances.push_back(hit.m_distance_in_voxel);
            },
            1.
        );
        ASSERT_EQ(1, visited_voxel_distances.size());
        EXPECT_EQ(1, visited_voxel_distances[0]);
    }

    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<double> visited_voxel_distances;
        lvox::algorithms::TraceBeamExactDistance{grid}(
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

TEST(grid, grid_traversal_rounding_distance_in_voxel)
{
    const double dim_x = 10;
    const double dim_y = 20;
    const double dim_z = 30;

    auto             pc = generate_cubic_point_cloud(dim_x, dim_y, dim_z);

    const double cell_size = 1.;
    lvox::Bounds point_cloud_bounds;
    for (const auto pt : pc)
    {
        point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());
    }

    lvox::BoundedGrid grid{point_cloud_bounds, cell_size};

    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<double> visited_voxel_distances;
        lvox::algorithms::TraceBeamVoxelRounding{grid}(
            beam,
            [&visited_voxel_distances](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_distances.push_back(hit.m_distance_in_voxel);
            },
            1.5
        );
        ASSERT_EQ(2, visited_voxel_distances.size());
        EXPECT_EQ(1, visited_voxel_distances[0]);
        EXPECT_EQ(1, visited_voxel_distances[1]);
    }

    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<double> visited_voxel_distances;
        lvox::algorithms::TraceBeamVoxelRounding{grid}(
            beam,
            [&visited_voxel_distances](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_distances.push_back(hit.m_distance_in_voxel);
            },
            1.
        );
        ASSERT_EQ(2, visited_voxel_distances.size());
        EXPECT_EQ(1, visited_voxel_distances[0]);
        EXPECT_EQ(1, visited_voxel_distances[1]);
    }

    {
        lvox::Vector pos{
            point_cloud_bounds.m_min_x, point_cloud_bounds.m_min_y, point_cloud_bounds.m_min_z
        };
        lvox::Vector dir{1., 0., 0.};
        lvox::Beam   beam{pos, dir};

        std::vector<double> visited_voxel_distances;
        lvox::algorithms::TraceBeamVoxelRounding{grid}(
            beam,
            [&visited_voxel_distances](const lvox::algorithms::VoxelHitInfo& hit) mutable {
                visited_voxel_distances.push_back(hit.m_distance_in_voxel);
            },
            0.5
        );
        ASSERT_EQ(1, visited_voxel_distances.size());
        EXPECT_EQ(1., visited_voxel_distances[0]);
    }
}

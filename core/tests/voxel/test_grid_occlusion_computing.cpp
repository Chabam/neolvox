#include <gtest/gtest.h>
#include <utils/utils.hpp>

#include <lvox/algorithms/trace_beam.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/grid.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/explore_grid.hpp>
#include <lvox/voxel/coo_grid.hpp>

TEST(grid, dense_occlusion_computing)
{
    lvox::Bounds<double> bounds{
        0., 10.,
        0., 1.,
        0., 1.
    };

    Point scan_origin{ 0.5, 0.5, 0.5};
    PointCloud pc{{ 1.5, 0.5, 0.5}};

    constexpr bool compute_variance = false;
    constexpr bool compute_occlusion = true;
    lvox::Grid grid = lvox::DenseGrid{bounds, 1., compute_variance, compute_occlusion};
    lvox::Scan<Point, PointCloud> s{pc, scan_origin};

    lvox::algorithms::ComputeOptions co{
        .m_voxel_size = 1.0,
        .m_job_limit = 1,
        .m_pad_estimator = lvox::algorithms::pad_estimators::BeerLambert{},
        .m_use_sparse_grid = false,
        .m_required_counts = 0,
        .m_smallest_element_area = 0,
        .m_use_classification = false,
        .m_compute_occlusion = compute_occlusion,
        .m_bounds = bounds,
        .m_log_stream = std::cout
    };

    lvox::algorithms::explore_grid<Point, PointCloud>(grid, s, co);

    lvox::COOGrid res = std::visit([](auto&& grid) {
        return lvox::COOGrid{grid};
    }, grid);

    for (auto v : res) {
        if (*v.x < 1)
            continue;

        EXPECT_EQ(1, *v.potential_counts);

        if (*v.x == 1)
            EXPECT_EQ(0.5, *v.potential_lengths);
        else
            EXPECT_EQ(1.0, *v.potential_lengths);

    }

}


TEST(grid, chunked_occlusion_computing)
{
    lvox::Bounds<double> bounds{
        0., 10.,
        0., 1.,
        0., 1.
    };

    Point scan_origin{ 0.5, 0.5, 0.5};
    PointCloud pc{{ 1.5, 0.5, 0.5}};

    constexpr bool compute_variance = false;
    constexpr bool compute_occlusion = true;
    lvox::Grid grid = lvox::ChunkedGrid{bounds, 1., compute_variance, compute_occlusion};
    lvox::Scan<Point, PointCloud> s{pc, scan_origin};

    lvox::algorithms::ComputeOptions co{
        .m_voxel_size = 1.0,
        .m_job_limit = 1,
        .m_pad_estimator = lvox::algorithms::pad_estimators::BeerLambert{},
        .m_use_sparse_grid = false,
        .m_required_counts = 0,
        .m_smallest_element_area = 0,
        .m_use_classification = false,
        .m_compute_occlusion = compute_occlusion,
        .m_bounds = bounds,
        .m_log_stream = std::cout
    };

    lvox::algorithms::explore_grid<Point, PointCloud>(grid, s, co);

    lvox::COOGrid res = std::visit([](auto&& grid) {
        return lvox::COOGrid{grid};
    }, grid);

    for (auto v : res) {
        if (*v.x < 1)
            continue;

        EXPECT_EQ(1, *v.potential_counts);

        if (*v.x == 1)
            EXPECT_EQ(0.5, *v.potential_lengths);
        else
            EXPECT_EQ(1.0, *v.potential_lengths);

    }

}

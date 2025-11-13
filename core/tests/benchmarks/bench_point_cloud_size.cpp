#include <benchmark/benchmark.h>
#include <memory>
#include <thread>
#include <utils/utils.hpp>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>
#include <pdal/util/Bounds.hpp>

#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/grid.hpp>

#include "lvox/logger/logger.hpp"

static void bm_point_cloud_size(benchmark::State& state, bool is_sparse)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        pdal::PointTable table;
        lvox::Logger     logger{"Benchmark"};
        logger.info("Generating point cloud");
        const auto view =
            generate_cubic_point_cloud_with_random_points(table, state.range(0), 30, 40, 20);

        pdal::BOX3D point_cloud_bounds;
        view->calculateBounds(point_cloud_bounds);

        using dim = pdal::Dimension::Id;

        logger.info("Converting to LVox format");
        auto lvox_point_cloud = std::make_unique<lvox::PointCloud>();
        for (const auto& pt : *view)
        {
            const double x        = pt.template getFieldAs<double>(dim::X);
            const double y        = pt.template getFieldAs<double>(dim::Y);
            const double z        = pt.template getFieldAs<double>(dim::Z);
            const double gps_time = pt.template getFieldAs<double>(dim::GpsTime);
            lvox_point_cloud->emplace_back(gps_time, lvox::Point{x, y, z});
        }

        std::vector<lvox::Scan> scans;
        scans.emplace_back(
            std::move(lvox_point_cloud),
            lvox::Point{0., 0., 0.},
            lvox::Bounds{
                point_cloud_bounds.minx,
                point_cloud_bounds.maxx,
                point_cloud_bounds.miny,
                point_cloud_bounds.maxy,
                point_cloud_bounds.minz,
                point_cloud_bounds.maxz
            }
        );
        lvox::algorithms::ComputeOptions options{
            .m_voxel_size           = 0.1,
            .m_job_limit            = std::thread::hardware_concurrency(),
            .m_pad_estimator        = lvox::algorithms::pad_estimators::BeerLambert{},
            .m_compute_theoriticals = false,
            .m_use_sparse_grid      = is_sparse
        };
        logger.info("Starting computation");
        lvox::Logger::set_global_level(lvox::Logger::Level::Error);

        state.ResumeTiming();
        std::ignore = lvox::algorithms::compute_pad(scans, options);
    }
}

BENCHMARK_CAPTURE(bm_point_cloud_size, sparse, true)
    ->Arg(10'000)
    ->Arg(100'000)
    ->Arg(1'000'000)
    ->Arg(10'000'000);
BENCHMARK_CAPTURE(bm_point_cloud_size, dense, false)
    ->Arg(10'000)
    ->Arg(100'000)
    ->Arg(1'000'000)
    ->Arg(10'000'000);

#include <benchmark/benchmark.h>
#include <thread>
#include <utils/utils.hpp>

#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/compute_pad.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/coo_grid.hpp>
#include <lvox/voxel/grid.hpp>

static void bm_point_cloud_size(benchmark::State& state, bool is_sparse)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        lvox::Logger logger{"Benchmark"};
        logger.info("Generating point cloud");
        const auto pc = generate_cubic_point_cloud_with_random_points(state.range(0), 30, 40, 20);

        lvox::Bounds point_cloud_bounds;

        for (const auto& pt : pc)
            point_cloud_bounds.grow(pt.x(), pt.y(), pt.z());

        std::vector<lvox::Scan<Point, PointCloud>> scans;
        scans.emplace_back(
            pc,
            Point{0., 0., 0.},
            point_cloud_bounds
        );
        lvox::algorithms::ComputeOptions options{
            .m_voxel_size           = 0.1,
            .m_job_limit            = std::thread::hardware_concurrency(),
            .m_pad_estimator        = lvox::algorithms::pad_estimators::BeerLambert{},
            .m_compute_theoriticals = false,
            .m_use_sparse_grid      = is_sparse,
            .m_log_stream           = std::cout
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

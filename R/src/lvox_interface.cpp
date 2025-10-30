// [[Rcpp::depends(lidR)]]

#include <Rcpp.h>
#include <lvox/types.hpp>
#include <lvox/algorithms/algorithms.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/voxel/bounds.hpp>
#include <thread>

using namespace Rcpp;

// [[Rcpp::export]]
void lvox_compute_pad(S4 point_cloud)
{
    if (!point_cloud.is("LAS"))
    {
        stop("compute pad supports only value from Rlas");
        return;
    }

    List          data     = point_cloud.slot("data");
    NumericVector xs       = data["X"];
    NumericVector ys       = data["Y"];
    NumericVector zs       = data["Z"];
    NumericVector gpstimes = data["gpstime"];

    lvox::PointCloudView points = std::make_unique<lvox::PointCloud>(xs.size());
    lvox::Bounds         bounds;
    for (size_t i = 0; i < points->size(); ++i)
    {
        (*points)[i] = lvox::TimedPoint{gpstimes[i], lvox::Point{xs[i], ys[i], zs[i]}};
        bounds.grow(xs[i], ys[i], zs[i]);
    }

    std::vector<lvox::Scan> scans;

    scans.push_back(lvox::Scan{std::move(points), lvox::Vector::Constant(0.), bounds, {}});

    namespace lvox_pe = lvox::algorithms::pad_estimators;
    const lvox::algorithms::ComputeOptions compute_options{
        .m_voxel_size           = 0.5,
        .m_job_limit            = std::thread::hardware_concurrency(),
        .m_pad_estimator        = lvox_pe::BeerLambert{},
        .m_compute_theoriticals = false,
        .m_use_sparse_grid      = false
    };

    lvox::algorithms::compute_pad(scans, compute_options);
}

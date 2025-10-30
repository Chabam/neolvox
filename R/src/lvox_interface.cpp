#include <Rcpp.h>
#include <lvox/types.hpp>
#include <lvox/algorithms/algorithms.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/voxel/bounds.hpp>
#include "Rcpp/DataFrame.h"
#include "Rcpp/vector/instantiation.h"
#include <thread>

using namespace Rcpp;

lvox::PointCloudView readPointCloudFromLidR(const Rcpp::S4& point_cloud)
{
    if (!point_cloud.is("LAS"))
    {
        stop("compute pad supports only value from Rlas");
    }

    const Rcpp::List&          data     = point_cloud.slot("data");
    const Rcpp::NumericVector& xs       = data["X"];
    const Rcpp::NumericVector& ys       = data["Y"];
    const Rcpp::NumericVector& zs       = data["Z"];
    const Rcpp::NumericVector& gpstimes = data["gpstime"];

    lvox::PointCloudView points = std::make_unique<lvox::PointCloud>(xs.size());
    for (size_t i = 0; i < points->size(); ++i)
    {
        (*points)[i] = lvox::TimedPoint{gpstimes[i], lvox::Point{xs[i], ys[i], zs[i]}};
    }

    return points;
}

// [[Rcpp::export]]
Rcpp::List lvox_compute_pad(
    const Rcpp::S4& point_cloud,
    double          voxel_size           = 0.5,
    bool            use_sparse_grid      = false,
    bool            compute_theoriticals = false,
    unsigned int    thread_count         = 8
)
{

    std::vector<lvox::Scan> scans;
    lvox::Bounds            bounds;
    lvox::PointCloudView    points = readPointCloudFromLidR(point_cloud);

    for (auto& pt : *points)
    {
        bounds.grow(pt.m_point.x(), pt.m_point.y(), pt.m_point.z());
    }

    scans.push_back(lvox::Scan{std::move(points), lvox::Vector::Constant(0.), bounds, {}});

    namespace lvox_pe = lvox::algorithms::pad_estimators;
    const lvox::algorithms::ComputeOptions compute_options{
        .m_voxel_size           = voxel_size,
        .m_job_limit            = thread_count,
        .m_pad_estimator        = lvox_pe::BeerLambert{},
        .m_compute_theoriticals = compute_theoriticals,
        .m_use_sparse_grid      = use_sparse_grid
    };

    lvox::COOGrid grid = lvox::algorithms::compute_pad(scans, compute_options);

    return Rcpp::List::create(
        Rcpp::Named("data") = Rcpp::DataFrame::create(
            Rcpp::Named("X")   = grid.xs(),
            Rcpp::Named("Y")   = grid.ys(),
            Rcpp::Named("Z")   = grid.zs(),
            Rcpp::Named("PAD") = grid.pads()
        ),
        Rcpp::Named("Voxel Size") = voxel_size,
        Rcpp::Named("Dimensions") = Rcpp::DoubleVector::create(
            grid.bounds().dim_x(), grid.bounds().dim_y(), grid.bounds().dim_z()
        ),
        Rcpp::Named("Minimal Coordinates") = Rcpp::DoubleVector::create(
            grid.bounds().bounds().m_min_x,
            grid.bounds().bounds().m_min_y,
            grid.bounds().bounds().m_min_z
        )
    );
}

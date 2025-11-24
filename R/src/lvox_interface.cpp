#include <Rcpp.h>
#include <lvox/types.hpp>
#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/compute_pad.hpp>
#include <lvox/voxel/coo_grid.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/scanner/trajectory.hpp>
#include <lvox/voxel/bounds.hpp>
#include <Rcpp/DataFrame.h>
#include <Rcpp/macros/module.h>
#include <Rcpp/vector/instantiation.h>

using namespace Rcpp;

namespace lvox_pe = lvox::algorithms::pad_estimators;

lvox::PointCloudView read_point_cloud_from_raw_data(const Rcpp::List& raw_data)
{
    const Rcpp::NumericVector& xs       = raw_data["X"];
    const Rcpp::NumericVector& ys       = raw_data["Y"];
    const Rcpp::NumericVector& zs       = raw_data["Z"];
    const Rcpp::NumericVector& gpstimes = raw_data["gpstime"];

    lvox::PointCloudView points = std::make_unique<lvox::PointCloud>(xs.size());
    for (size_t i = 0; i < points->size(); ++i)
    {
        (*points)[i] = lvox::TimedPoint{gpstimes[i], lvox::Point{xs[i], ys[i], zs[i]}};
    }

    return points;
}

lvox::PointCloudView try_read_point_cloud_as_las(const SEXP& expr)
{
    try
    {
        Rcpp::S4 las{expr};
        if (!las.is("LAS"))
            stop("Point cloud data type is not supported");

        return read_point_cloud_from_raw_data(las.slot("data"));
    }
    catch (std::exception _)
    {
        // We tried ¯\_(ツ)_/¯
    }

    return read_point_cloud_from_raw_data(expr);
}

lvox_pe::PADEstimator get_estimator_from_string(std::string pe)
{
    std::ranges::transform(pe, pe.begin(), [](const unsigned char c) {
        return std::toupper(c);
    });

    if (pe == "BL")
        return lvox_pe::BeerLambert{};
    else if (pe == "CF")
        return lvox_pe::ContactFrequency{};
    else if (pe == "UPLBL")
        return lvox_pe::UnequalPathLengthBeerLambert{};
    else if (pe == "BCMLE")
        return lvox_pe::BiasCorrectedMaximumLikelyhoodEstimator{};
    else
        stop("Unkown PAD estimator ");
}

Rcpp::List do_lvox_computation(
    const std::vector<lvox::Scan>& scans,
    std::string                    padEstimator,
    double                         voxelSize,
    bool                           useSparseGrid,
    unsigned int                   requiredHits,
    unsigned int                   threadCount,
    bool                           exportAllGridMetadata
)
{
    const lvox::algorithms::ComputeOptions compute_options{
        .m_voxel_size           = voxelSize,
        .m_job_limit            = threadCount,
        .m_pad_estimator        = get_estimator_from_string(padEstimator),
        .m_compute_theoriticals = false, // TODO: support theoriticals
        .m_use_sparse_grid      = useSparseGrid
    };

    lvox::COOGrid   grid = lvox::algorithms::compute_pad(scans, compute_options);
    Rcpp::DataFrame df   = Rcpp::DataFrame::create(
        Rcpp::Named("X")   = grid.xs(),
        Rcpp::Named("Y")   = grid.ys(),
        Rcpp::Named("Z")   = grid.zs(),
        Rcpp::Named("PAD") = grid.pads()
    );

    if (exportAllGridMetadata)
    {
        df.push_back(grid.hits(), "Hits");
        df.push_back(grid.lengths(), "Lengths");
        df.push_back(grid.hits_lengths(), "Hits Lengths");
        if (!grid.lengths_variance().empty())
            df.push_back(grid.lengths_variance(), "Lengths variance");
    }

    return Rcpp::List::create(
        Rcpp::Named("data")       = df,
        Rcpp::Named("Voxel Size") = voxelSize,
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

// [[Rcpp::depends(RcppParallel)]]

//' Perform the PAD estimation for a MLS scan
//'
//' @param pointCloud A point cloud acquired from a mobile lidar, this can be LAS object or a list containing X, Y, Z and gpstime.
//' @param trajectory A trajectory point cloud acquired from a mobile lidar, this can be LAS object or a list containing X, Y, Z and gpstime.
//' @param padEstimator Accronym for the PAD estimator to use (BL, CF, UPLBL and BCMLE)
//' @param voxelSize The size of the voxels in the grid in meters
//' @param useSparseGrid Whether or not to use sparse grid for computation, it should take less memory
//' @param requiredHits The number of return required for PAD computation, if the return amount in the voxel is lower than this number it will be excluded from the estimation
//' @param threadCount The amount of parallel processing thread to use. Set this to your core count for best performance.
//' @param exportAllGridMetadata Whether or not to export all intermediate data from the Lvox computation
//' @return A list containing the 3d grid in a coordinate list (COO) form. It also contains metadata about the grid (voxel size, grid dimensions, etc.)
// [[Rcpp::export]]
Rcpp::List lvoxComputeMLS(
    const SEXP&       pointCloud,
    const Rcpp::List& trajectory,
    std::string       padEstimator          = "BCMLE",
    double            voxelSize             = 0.5,
    bool              useSparseGrid         = false,
    unsigned int      requiredHits          = 5,
    bool              exportAllGridMetadata = false,
    unsigned int      threadCount           = 8
)
{
    lvox::Bounds         bounds;
    lvox::PointCloudView points = try_read_point_cloud_as_las(pointCloud);

    for (auto& pt : *points)
    {
        bounds.grow(pt.m_point.x(), pt.m_point.y(), pt.m_point.z());
    }

    std::vector<lvox::Scan> scans;
    scans.push_back(
        lvox::Scan{
            std::move(points),
            std::make_shared<lvox::Trajectory>(read_point_cloud_from_raw_data(trajectory)),
            bounds,
            {}
        }
    );

    return do_lvox_computation(
        scans,
        padEstimator,
        voxelSize,
        requiredHits,
        useSparseGrid,
        threadCount,
        exportAllGridMetadata
    );
}

//' Perform the PAD estimation for TLS multi-scans
//'
//' @param pointClouds A list of point clouds acquired from a mobile lidar, this can be  LAS object or a list containing X, Y, Z and gpstime.
//' @param scannersOrigin A list of corresponding scanner's origins coordinates.
//' @param padEstimator Accronym for the PAD estimator to use (BL, CF, UPLBL and BCMLE)
//' @param voxelSize The size of the voxels in the grid in meters
//' @param useSparseGrid Whether or not to use sparse grid for computation, it should take less memory
//' @param requiredHits The number of return required for PAD computation, if the return amount in the voxel is lower than this number it will be excluded from the estimation
//' @param threadCount The amount of parallel processing thread to use. Set this to your core count for best performance.
//' @param exportAllGridMetadata Whether or not to export all intermediate data from the Lvox computation
//' @return A list containing the 3d grid in a coordinate list (COO) form. It also contains metadata about the grid (voxel size, grid dimensions, etc.)
// [[Rcpp::export]]
Rcpp::List lvoxComputeTLS(
    const Rcpp::List& pointClouds,
    const Rcpp::List& scannersOrigin,
    std::string       padEstimator          = "BCMLE",
    double            voxelSize             = 0.5,
    unsigned int      requiredHits          = 5,
    bool              useSparseGrid         = false,
    bool              exportAllGridMetadata = false,
    unsigned int      threadCount           = 8
)
{
    if (pointClouds.size() != scannersOrigin.size())
        stop(
            "The amount of provided pointClouds doesn't match the amount of provided scanner's "
            "origin"
        );

    lvox::Bounds            bounds;
    std::vector<lvox::Scan> scans;
    for (size_t i = 0; i < pointClouds.size(); ++i)
    {
        lvox::PointCloudView points = try_read_point_cloud_as_las(pointClouds[i]);

        for (auto& pt : *points)
        {
            bounds.grow(pt.m_point.x(), pt.m_point.y(), pt.m_point.z());
        }

        const Rcpp::DoubleVector& scanners_origin = scannersOrigin[i];

        scans.push_back(
            lvox::Scan{
                std::move(points),
                lvox::Point{scanners_origin[0], scanners_origin[1], scanners_origin[2]},
                bounds,
                {}
            }
        );
    }

    return do_lvox_computation(
        scans,
        padEstimator,
        voxelSize,
        requiredHits,
        useSparseGrid,
        threadCount,
        exportAllGridMetadata
    );
}

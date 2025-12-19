#include <Rcpp.h>
#include <Rcpp/DataFrame.h>
#include <Rcpp/macros/module.h>
#include <Rcpp/vector/instantiation.h>

#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/compute_pad.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/scanner/trajectory.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/bounds.hpp>
#include <lvox/voxel/coo_grid.hpp>

using namespace Rcpp;

namespace lvox_pe = lvox::algorithms::pad_estimators;

struct Point
{
    double x() const { return m_x; }
    double y() const { return m_y; }
    double z() const { return m_z; }
    double gps_time() const { return m_gps_time; }

    double m_x;
    double m_y;
    double m_z;
    double m_gps_time;
};

struct PointCloud
{
    PointCloud(
        const Rcpp::NumericVector& xs,
        const Rcpp::NumericVector& ys,
        const Rcpp::NumericVector& zs,
        const Rcpp::NumericVector& gps_times
    )
        : m_count{static_cast<size_t>(xs.size())}
        , m_xs{xs}
        , m_ys{ys}
        , m_zs{zs}
        , m_gps_times{gps_times}
    {
    }

    class PointIterator
    {
        friend class PointCloud;

      public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = Point;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;
        using const_reference   = const value_type&;

        PointIterator()                     = default;
        PointIterator(const PointIterator&) = default;

        PointIterator& operator=(const PointIterator&) = default;

        PointIterator& operator++()
        {
            *this += 1;
            return *this;
        }

        PointIterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        PointIterator& operator--()
        {
            *this -= 1;
            return *this;
        }

        PointIterator operator--(int)
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        PointIterator operator+(difference_type diff) const
        {
            auto tmp = *this;
            tmp.m_index += diff;
            tmp.change_index();
            return tmp;
        }

        friend PointIterator operator+(difference_type other, const PointIterator& diff)
        {
            return other + diff;
        }

        PointIterator operator-(difference_type diff) const
        {
            auto tmp = *this - diff;
            tmp.m_index -= diff;
            tmp.change_index();
            return tmp;
        }

        difference_type operator-(const PointIterator& other) const
        {
            return m_index - other.m_index;
        }

        PointIterator& operator+=(difference_type diff)
        {
            m_index += diff;
            change_index();
            return *this;
        }

        PointIterator& operator-=(difference_type diff)
        {
            m_index -= diff;
            change_index();
            return *this;
        }

        value_type operator*() const { return *m_value; }
        pointer    operator->() { return &(*m_value); }

        bool operator==(const PointIterator& other) const
        {
            return m_point_cloud == other.m_point_cloud && m_index == other.m_index;
        }

        bool operator!=(const PointIterator& other) const { return !(*this == other); }

        bool operator<(const PointIterator& other) const { return m_index < other.m_index; }
        bool operator>(const PointIterator& other) const { return m_index > other.m_index; }

        bool operator<=(const PointIterator& other) const { return !(*this > other); }
        bool operator>=(const PointIterator& other) const { return !(*this < other); }

        value_type operator[](difference_type diff) const { return *(*this + diff); }

      private:
        size_t                    m_index;
        const PointCloud*         m_point_cloud;
        std::optional<value_type> m_value;

        PointIterator(const PointCloud* point_cloud, size_t idx)
            : m_index{idx}
            , m_point_cloud{point_cloud}
            , m_value{std::invoke([this]() -> std::optional<value_type> {
                if (m_index < m_point_cloud->m_count)
                    return value_type{
                        m_point_cloud->m_xs[m_index],
                        m_point_cloud->m_ys[m_index],
                        m_point_cloud->m_zs[m_index],
                        m_point_cloud->m_gps_times[m_index]
                    };
                else
                    return {};
            })}
        {
        }

        void change_index()
        {
            if (m_index >= m_point_cloud->m_count)
            {
                m_value.reset();
                return;
            }

            m_value.emplace(
                m_point_cloud->m_xs[m_index],
                m_point_cloud->m_ys[m_index],
                m_point_cloud->m_zs[m_index],
                m_point_cloud->m_gps_times[m_index]
            );
        }
    };

    using const_iterator = PointIterator;

    static_assert(std::random_access_iterator<const_iterator>);

    const_iterator begin() const { return const_iterator{this, 0}; }
    const_iterator end() const { return const_iterator{this, m_count}; }
    size_t         size() const { return m_count; }

    size_t             m_count;
    Rcpp::DoubleVector m_xs;
    Rcpp::DoubleVector m_ys;
    Rcpp::DoubleVector m_zs;
    Rcpp::DoubleVector m_gps_times;
};

using Scan          = lvox::Scan<Point, PointCloud>;
using ScannerOrigin = lvox::ScannerOrigin<Point, PointCloud>;
using Trajectory    = lvox::Trajectory<Point, PointCloud>;

PointCloud read_point_cloud_from_raw_data(const Rcpp::List& raw_data)
{
    return PointCloud{raw_data["X"], raw_data["Y"], raw_data["Z"], raw_data["gpstime"]};
}

PointCloud try_read_point_cloud_as_las(const SEXP& expr)
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
    const std::vector<Scan>& scans,
    std::string              padEstimator,
    double                   voxelSize,
    bool                     useSparseGrid,
    unsigned int             requiredHits,
    unsigned int             threadCount,
    bool                     exportAllGridMetadata
)
{
    const lvox::algorithms::ComputeOptions compute_options{
        .m_voxel_size           = voxelSize,
        .m_job_limit            = threadCount,
        .m_pad_estimator        = get_estimator_from_string(padEstimator),
        .m_compute_theoriticals = false, // TODO: support theoriticals
        .m_use_sparse_grid      = useSparseGrid,
        .m_log_stream           = Rcpp::Rcout
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
        df.push_back(grid.hits_lengths(), "HitsLengths");
        if (!grid.lengths_variance().empty())
            df.push_back(grid.lengths_variance(), "LengthsVariance");
    }

    auto new_grid = Rcpp::List::create(
        Rcpp::Named("data")       = df,
        Rcpp::Named("VoxelSize") = voxelSize,
        Rcpp::Named("Dimensions") = Rcpp::DoubleVector::create(
            grid.bounds().dim_x(), grid.bounds().dim_y(), grid.bounds().dim_z()
        ),
        Rcpp::Named("MinimalCoords") = Rcpp::DoubleVector::create(
            grid.bounds().bounds().m_min_x,
            grid.bounds().bounds().m_min_y,
            grid.bounds().bounds().m_min_z
        )
    );

    new_grid.attr("class") = "LVoxGrid";

    return new_grid;
}

// [[Rcpp::depends(RcppParallel)]]

//' Perform the PAD estimation for a MLS scan
//'
//' @param pointCloud A point cloud acquired from a mobile lidar, this can be LAS object or a list containing X, Y, Z and gpstime.
//' @param trajectory A trajectory point cloud acquired from a mobile lidar, this can be LAS object or a list containing X, Y, Z and gpstime.
//' @param padEstimator Accronym for the PAD estimator to use (BL, CF, UPLBL and BCMLE)
//' @param voxelSize The size of the voxels in the grid in meters
//' @param useSparseGrid Whether or not to use sparse grid for computation, it should take less memory.
//' @param requiredHits The number of return required for PAD computation, if the return amount in the voxel is lower than this number it will be excluded from the estimation
//' @param threadCount The amount of parallel processing thread to use. Set this to your core count for best performance.
//' @param exportAllGridMetadata Whether or not to export all intermediate data from the Lvox computation.
//' @return A LvoxGrid object containing the 3d grid in a coordinate list (COO) form. It also contains metadata about the grid (voxel size, grid dimensions, etc.)
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
    lvox::Bounds bounds;
    PointCloud   points = try_read_point_cloud_as_las(pointCloud);

    for (const auto& pt : points)
    {
        bounds.grow(pt.x(), pt.y(), pt.z());
    }

    std::vector<Scan> scans;
    scans.emplace_back(points, read_point_cloud_from_raw_data(trajectory), bounds);

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
//' @param useSparseGrid Whether or not to use sparse grid for computation, it should take less memory.
//' @param requiredHits The number of return required for PAD computation, if the return amount in the voxel is lower than this number it will be excluded from the estimation
//' @param threadCount The amount of parallel processing thread to use. Set this to your core count for best performance.
//' @param exportAllGridMetadata Whether or not to export all intermediate data from the Lvox computation.
//' @return A LvoxGrid object containing the 3d grid in a coordinate list (COO) form. It also contains metadata about the grid (voxel size, grid dimensions, etc.)
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
    std::vector<Scan>       scans;
    std::vector<PointCloud> point_clouds;
    scans.reserve(pointClouds.size());
    point_clouds.reserve(pointClouds.size());
    for (size_t i = 0; i < pointClouds.size(); ++i)
    {
        point_clouds.emplace_back(try_read_point_cloud_as_las(pointClouds[i]));

        for (const auto& pt : point_clouds[i])
        {
            bounds.grow(pt.x(), pt.y(), pt.z());
        }

        Rcpp::DoubleVector current_origin = scannersOrigin[i];

        scans.emplace_back(
            point_clouds[i],
            Point{current_origin[0], current_origin[1], current_origin[2], 0},
            bounds
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
// clang-format on

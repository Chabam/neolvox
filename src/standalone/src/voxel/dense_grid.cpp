#include <H5Cpp.h>
#include <H5Fpublic.h>
#include <atomic>
#include <format>
#include <iterator>
#include <numbers>
#include <ranges>

#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/chunked_grid.hpp>
#include <lvox/voxel/dense_grid.hpp>

namespace lvox
{

DenseGrid::DenseGrid(const Bounds& bounds, double cell_size, bool compute_variance)
    : m_bounded_grid{bounds, cell_size}
    , m_hits{m_bounded_grid.m_cell_count, std::allocator<std::atomic_uint>{}}
    , m_counts{m_bounded_grid.m_cell_count, std::allocator<std::atomic_uint>{}}
    , m_lengths{m_bounded_grid.m_cell_count, std::allocator<atomic_f64>{}}
    , m_hits_lengths{m_bounded_grid.m_cell_count, std::allocator<atomic_f64>{}}
    , m_lengths_variance{std::invoke([&]() -> std::vector<atomic_f64> {
        if (compute_variance)
            return std::vector<atomic_f64>{
                m_bounded_grid.m_cell_count, std::allocator<atomic_f64>{}
            };
        else
            return {};
    })}
    , m_pad{m_bounded_grid.m_cell_count, std::allocator<atomic_f64>{}}
{
}

DenseGrid::DenseGrid(DenseGrid&& other)
    : m_bounded_grid{std::move(other.m_bounded_grid)}
    , m_hits{std::move(other.m_hits)}
    , m_counts{std::move(other.m_counts)}
    , m_lengths{std::move(other.m_lengths)}
    , m_hits_lengths{std::move(other.m_lengths_variance)}
    , m_lengths_variance{std::move(other.m_lengths_variance)}
    , m_pad{std::move(other.m_pad)}

{
}

auto DenseGrid::index3d_to_flat_idx(const Index3D& voxel_idx) const -> size_t
{
    const auto& [x, y, z] = voxel_idx;

    return x + y * m_bounded_grid.dim_x() + z * m_bounded_grid.dim_x() * m_bounded_grid.dim_y();
}

auto DenseGrid::register_hit(const Index3D& voxel_idx) -> void
{
    auto idx = index3d_to_flat_idx(voxel_idx);
    m_hits[idx].fetch_add(1, std::memory_order_relaxed);
}

auto DenseGrid::add_length_and_count(const Index3D& voxel_idx, double length, bool is_hit) -> void
{
    auto idx = index3d_to_flat_idx(voxel_idx);

    m_lengths[idx].fetch_add(length, std::memory_order_relaxed);

    if (is_hit)
        m_hits_lengths[idx].fetch_add(length, std::memory_order_relaxed);

    m_counts[idx].fetch_add(1, std::memory_order_relaxed);
}

// TODO: make this work with memory consistency
auto DenseGrid::add_length_count_and_variance(const Index3D& voxel_idx, double length, bool is_hit) -> void
{
    auto idx = index3d_to_flat_idx(voxel_idx);

    const double previous_lengths = m_lengths[idx].fetch_add(length, std::memory_order_acq_rel);
    const double previous_counts  = m_counts[idx].fetch_add(1, std::memory_order_acq_rel);

    // No variance possible if the count is not big enough
    if (previous_counts < 2)
        return;

    // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
    const double previous_mean = previous_lengths / previous_counts;
    const double delta         = length - previous_mean;
    const double new_count     = previous_counts + 1;
    const double new_mean      = previous_mean + (delta / new_count);
    const double delta_2       = length - new_mean;

    // -1 on the count here because we are computing a variance sample.
    m_lengths_variance[idx].fetch_add((delta * delta_2) / (new_count - 1), std::memory_order_acq_rel);
}

auto DenseGrid::compute_pad(algorithms::pe::BeerLambert) -> void
{
    compute_pad_impl([this](unsigned int voxel_idx) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits            = m_hits[voxel_idx];
        const double ray_count       = m_counts[voxel_idx];
        const double RDI             = hits / static_cast<double>(ray_count);
        const double ray_length      = m_lengths[voxel_idx];
        const double mean_ray_length = ray_length / ray_count;

        if (RDI >= 1.)
            return 0.;

        return -(std::log(1. - RDI) / G(mean_ray_length));
    });
}

auto DenseGrid::compute_pad(algorithms::pe::ContactFrequency) -> void
{
    compute_pad_impl([this](unsigned int voxel_idx) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits       = m_hits[voxel_idx];
        const double ray_count  = m_counts[voxel_idx];
        const double RDI        = hits / static_cast<double>(ray_count);
        const double ray_length = m_lengths[voxel_idx];

        if (RDI >= 1.)
            return 0.;

        return RDI / G(ray_length);
    });
}

auto DenseGrid::compute_pad(algorithms::pe::UnequalPathLengthBeerLambert) -> void
{
    compute_pad_impl([this](unsigned int voxel_idx) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits               = m_hits[voxel_idx];
        const double ray_count          = m_counts[voxel_idx];
        const double RDI                = hits / static_cast<double>(ray_count);
        const double length             = m_lengths[voxel_idx];
        const double variance           = m_lengths_variance[voxel_idx];
        const double mean_ray_length    = length / ray_count;
        const double unequal_path_ratio = variance / mean_ray_length;

        double attenuation_coeff;

        if (RDI < 1.)
        {
            const double inv_RDI = 1. - RDI;
            attenuation_coeff =
                (1. / mean_ray_length) * (std::log(inv_RDI) - (1. / (2 * ray_count * inv_RDI)));
        }
        else // RDI == 1.
        {
            attenuation_coeff = std::log(2 * ray_count + 2) / mean_ray_length;
        }

        const double res = (1. / G(attenuation_coeff)) *
                           (1. - std::sqrt(1. - 2 * unequal_path_ratio * attenuation_coeff));

        if (std::isnan(res))
        {
            std::cout << std::format(
                             R"(
    hits                = {}
    ray_count           = {}
    RDI                 = {}
    ray_length          = {}
    mean_ray_length     = {}
    ray_length_variance = {}
    unequal_path_ratio  = {}
    attenuation_coeff   = {})",
                             hits,
                             ray_count,
                             RDI,
                             length,
                             mean_ray_length,
                             variance,
                             unequal_path_ratio,
                             attenuation_coeff
                         )
                      << std::endl;
            return 0.;
        }

        return res;
    });
}

auto DenseGrid::export_as_coo_to_h5(
    const std::string& dataset_name, const std::filesystem::path& filename, bool include_all_data
) const -> void
{
    Logger     logger{"Grid HDF5 export"};
    H5::H5File file;
    if (std::filesystem::exists(filename))
        std::filesystem::remove(filename);

    file = H5::H5File{filename.string(), H5F_ACC_TRUNC};

    std::vector<unsigned int> xs;
    std::vector<unsigned int> ys;
    std::vector<unsigned int> zs;
    std::vector<unsigned int> counts;
    std::vector<unsigned int> hits;
    std::vector<double>       pads;
    std::vector<double>       lengths;
    std::vector<double>       lengths_variance;

    std::vector<unsigned int> all_indexes;
    all_indexes.resize(m_counts.size());
    std::iota(all_indexes.begin(), all_indexes.end(), 0);
    auto index_with_data = all_indexes | std::views::filter([this](auto idx) -> bool {
                               return m_counts[idx] != 0;
                           });
    std::vector<Index3D> index3d_with_data;
    std::ranges::transform(
        index_with_data, std::back_inserter(index3d_with_data), [this](size_t idx) -> Index3D {
            const auto dim_x = m_bounded_grid.m_dim_x;
            const auto dim_y = m_bounded_grid.m_dim_y;
            const auto dim_z = m_bounded_grid.m_dim_z;

            return {
                static_cast<unsigned int>(idx % dim_x),
                static_cast<unsigned int>((idx / dim_x) % dim_x),
                static_cast<unsigned int>(idx / (dim_x * dim_y))
            };
        }
    );

    // Copying indexes
    const auto h5_size_t = H5::PredType::NATIVE_UINT;
    std::ranges::copy(index3d_with_data | std::views::elements<0>, std::back_inserter(xs));
    std::ranges::copy(index3d_with_data | std::views::elements<1>, std::back_inserter(ys));
    std::ranges::copy(index3d_with_data | std::views::elements<2>, std::back_inserter(zs));
    std::ranges::copy(
        index_with_data | std::views::transform([this](const size_t& index) -> double {
            return m_pad[index];
        }),
        std::back_inserter(pads)
    );

    if (include_all_data)
    {
        std::ranges::copy(
            index_with_data | std::views::transform([this](const size_t& index) -> unsigned int {
                return m_hits[index];
            }),
            std::back_inserter(hits)
        );
        std::ranges::copy(
            index_with_data | std::views::transform([this](const size_t& index) -> unsigned int {
                return m_counts[index];
            }),
            std::back_inserter(counts)
        );
    }

    {
        std::ranges::copy(
            index_with_data | std::views::transform([this](const size_t& index) -> unsigned int {
                return m_lengths[index];
            }),
            std::back_inserter(lengths)
        );
    }

    if (!m_lengths_variance.empty())
    {
        auto chunk_lengths_variance =
            index_with_data | std::views::transform([this](const size_t& index) -> unsigned int {
                return m_lengths_variance[index];
            });
        std::copy(
            chunk_lengths_variance.begin(),
            chunk_lengths_variance.end(),
            std::back_inserter(lengths_variance)
        );
    }

    // TODO: refactor this

    const hsize_t voxels_with_data = std::ranges::distance(pads);

    using h5_dimension_t = std::array<hsize_t, 1>;
    const h5_dimension_t voxel_count_dim{voxels_with_data};

    const H5::DataSpace data_space{std::tuple_size_v<h5_dimension_t>, voxel_count_dim.data()};

    H5::Group plot_group;
    if (file.nameExists(dataset_name))
        plot_group = file.openGroup(dataset_name);
    else
        plot_group = file.createGroup(dataset_name, 8);

    H5::DSetCreatPropList create_prop_list{};
    h5_dimension_t        chunk_dims{std::min(static_cast<hsize_t>(2 << 13), voxels_with_data)};
    create_prop_list.setChunk(1, chunk_dims.data());

    const auto get_or_create_dataset =
        [&plot_group, &dataset_name, &create_prop_list](
            const std::string& name, const H5::PredType& type, const H5::DataSpace& dataspace
        ) -> H5::DataSet {
        if (plot_group.nameExists(name))
        {
            return plot_group.openDataSet(name);
        }

        return plot_group.createDataSet(name, type, dataspace, create_prop_list);
    };

    logger.info("Exporting grid to '{}'", filename.string());

    const H5::PredType h5_index_t = H5::PredType::NATIVE_UINT;
    {
        H5::DataSet xs_data = get_or_create_dataset("x", h5_index_t, data_space);
        xs_data.write(xs.data(), h5_index_t);
    }

    {
        H5::DataSet ys_data = get_or_create_dataset("y", h5_index_t, data_space);
        ys_data.write(ys.data(), h5_index_t);
    }

    {
        H5::DataSet zs_data = get_or_create_dataset("z", h5_index_t, data_space);
        zs_data.write(zs.data(), h5_index_t);
    }

    {
        H5::PredType h5_pad_t    = H5::PredType::NATIVE_DOUBLE;
        H5::DataSet  values_data = get_or_create_dataset("pad", h5_pad_t, data_space);
        values_data.write(pads.data(), h5_pad_t);
    }

    if (include_all_data)
    {
        {
            H5::PredType h5_hits_t = H5::PredType::NATIVE_UINT;
            H5::DataSet  hits_data = get_or_create_dataset("hits", h5_hits_t, data_space);
            hits_data.write(hits.data(), h5_hits_t);
        }

        {
            H5::PredType h5_counts_t = H5::PredType::NATIVE_UINT;
            H5::DataSet  counts_data = get_or_create_dataset("counts", h5_counts_t, data_space);
            counts_data.write(counts.data(), h5_counts_t);
        }

        {
            H5::PredType h5_lengths_t = H5::PredType::NATIVE_DOUBLE;
            H5::DataSet  lengths_data = get_or_create_dataset("lengths", h5_lengths_t, data_space);
            lengths_data.write(lengths.data(), h5_lengths_t);
        }

        if (!lengths_variance.empty())
        {
            H5::PredType h5_lengths_var_t = H5::PredType::NATIVE_DOUBLE;
            H5::DataSet  length_variances_data =
                get_or_create_dataset("lengths variance", h5_lengths_var_t, data_space);
            length_variances_data.write(lengths_variance.data(), h5_lengths_var_t);
        }
    }

    // Minimum coordinate attribute
    const std::array<hsize_t, 1> scalar_value_dim{1};
    H5::DataSpace                scalar_data_space{1, scalar_value_dim.data()};
    const std::array<hsize_t, 1> singular_3d_coord_count = {3};
    H5::DataSpace                singular_3d_coord_data_space{1, singular_3d_coord_count.data()};

    const auto get_or_create_attribute =
        [&plot_group](
            const std::string& name, const H5::PredType& type, const H5::DataSpace& dataspace
        ) -> H5::Attribute {
        if (plot_group.attrExists(name))
        {
            return plot_group.openAttribute(name);
        }

        return plot_group.createAttribute(name, type, dataspace);
    };
    const auto    h5_voxel_size_t = H5::PredType::NATIVE_DOUBLE;
    H5::Attribute voxel_size_attr =
        get_or_create_attribute("Voxel size", h5_voxel_size_t, H5::DataSpace{});
    voxel_size_attr.write(h5_voxel_size_t, &m_bounded_grid.m_cell_size);

    const auto h5_min_coords_t = H5::PredType::NATIVE_DOUBLE;

    H5::Attribute min_coord_attr = get_or_create_attribute(
        "Minimal coordinates values", h5_min_coords_t, singular_3d_coord_data_space
    );

    const std::array<double, 3> min_coords = {
        m_bounded_grid.m_bounds.minx, m_bounded_grid.m_bounds.miny, m_bounded_grid.m_bounds.minz
    };
    min_coord_attr.write(h5_voxel_size_t, min_coords.data());

    // Grid dimensions
    const auto    h5_grid_dim_t = H5::PredType::NATIVE_UINT64;
    H5::Attribute grid_dims_attr =
        get_or_create_attribute("Dimensions", h5_grid_dim_t, singular_3d_coord_data_space);
    const std::array<size_t, 3> grid_dims = {
        m_bounded_grid.m_dim_x, m_bounded_grid.m_dim_y, m_bounded_grid.m_dim_z
    };
    grid_dims_attr.write(h5_grid_dim_t, grid_dims.data());

    file.close();
}

} // namespace lvox

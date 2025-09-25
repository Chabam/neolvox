#include <H5Cpp.h>
#include <iterator>
#include <ranges>

#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/voxel/chunked_grid.hpp>
#include <lvox/voxel/coo_grid.hpp>
#include <lvox/voxel/dense_grid.hpp>

namespace lvox
{

COOGrid::COOGrid(const ChunkedGrid& grid)
    : m_xs{}
    , m_ys{}
    , m_zs{}
    , m_counts{}
    , m_hits{}
    , m_pads{}
    , m_lengths{}
    , m_hits_lengths{}
    , m_lengths_variance{}
    , m_bounded_grid{grid.m_bounded_grid}
{
    auto chunk_idx = 0;
    for (const auto& a_chunk : grid.m_chunks)
    {
        auto chunk = a_chunk.load(std::memory_order_relaxed);

        if (!chunk)
            continue;

        const auto chunk_x = static_cast<unsigned int>(chunk_idx % grid.m_chunks_x);
        const auto chunk_y =
            static_cast<unsigned int>((chunk_idx / grid.m_chunks_x) % grid.m_chunks_y);
        const auto chunk_z =
            static_cast<unsigned int>(chunk_idx / (grid.m_chunks_x * grid.m_chunks_y));

        std::vector<unsigned int> all_indexes;
        all_indexes.resize(chunk->m_counts.size());
        std::iota(all_indexes.begin(), all_indexes.end(), 0);
        auto index_with_data = all_indexes | std::views::filter([&chunk](auto idx) -> bool {
                                   return chunk->m_counts[idx] != 0;
                               });
        std::vector<Index3D> index3d_with_data;
        std::ranges::transform(
            index_with_data,
            std::back_inserter(index3d_with_data),
            [&grid, chunk_x, chunk_y, chunk_z](size_t idx) -> Index3D {
                constexpr auto chunk_dim = ChunkedGrid::VoxelChunk::s_edge_size;
                return {
                    chunk_x * chunk_dim + static_cast<unsigned int>(idx % chunk_dim),
                    chunk_y * chunk_dim + static_cast<unsigned int>((idx / chunk_dim) % chunk_dim),
                    chunk_z * chunk_dim + static_cast<unsigned int>(idx / (chunk_dim * chunk_dim))
                };
            }
        );

        // Copying indexes
        const auto h5_size_t = H5::PredType::NATIVE_UINT;
        std::ranges::copy(index3d_with_data | std::views::elements<0>, std::back_inserter(m_xs));
        std::ranges::copy(index3d_with_data | std::views::elements<1>, std::back_inserter(m_ys));
        std::ranges::copy(index3d_with_data | std::views::elements<2>, std::back_inserter(m_zs));

        std::ranges::copy(
            index_with_data | std::views::transform([&chunk](const size_t& index) -> unsigned int {
                return chunk->m_hits[index];
            }),
            std::back_inserter(m_hits)
        );
        std::ranges::copy(
            index_with_data | std::views::transform([&chunk](const size_t& index) -> unsigned int {
                return chunk->m_counts[index];
            }),
            std::back_inserter(m_counts)
        );

        {
            std::ranges::copy(
                index_with_data |
                    std::views::transform([&chunk](const size_t& index) -> unsigned int {
                        return chunk->m_lengths[index];
                    }),
                std::back_inserter(m_lengths)
            );
        }

        {
            std::ranges::copy(
                index_with_data |
                    std::views::transform([&chunk](const size_t& index) -> unsigned int {
                        return chunk->m_hits_lengths[index];
                    }),
                std::back_inserter(m_hits_lengths)
            );
        }

        if (!chunk->m_lengths_variance.empty())
        {
            auto chunk_lengths_variance =
                index_with_data |
                std::views::transform([&chunk](const size_t& index) -> unsigned int {
                    return chunk->m_lengths_variance[index];
                });
            std::copy(
                chunk_lengths_variance.begin(),
                chunk_lengths_variance.end(),
                std::back_inserter(m_lengths_variance)
            );
        }
        ++chunk_idx;
    }
    m_pads.resize(m_counts.size());
    assert(
        m_xs.size() == m_ys.size() && m_ys.size() == m_counts.size() &&
        m_counts.size() == m_hits.size() && m_hits.size() == m_pads.size() &&
        m_pads.size() == m_lengths.size() && m_lengths.size() == m_hits_lengths.size()
    );
}

COOGrid::COOGrid(const DenseGrid& grid)
    : m_xs{}
    , m_ys{}
    , m_zs{}
    , m_counts{}
    , m_hits{}
    , m_pads{}
    , m_lengths{}
    , m_hits_lengths{}
    , m_lengths_variance{}
    , m_bounded_grid{grid.m_bounded_grid}
{
    std::vector<unsigned int> all_indexes;
    all_indexes.resize(grid.m_counts.size());
    std::iota(all_indexes.begin(), all_indexes.end(), 0);
    auto index_with_data = all_indexes | std::views::filter([&grid](auto idx) -> bool {
                               return grid.m_counts[idx] != 0;
                           });
    std::vector<Index3D> index3d_with_data;
    std::ranges::transform(
        index_with_data, std::back_inserter(index3d_with_data), [&grid](size_t idx) -> Index3D {
            const auto dim_x = grid.m_bounded_grid.m_dim_x;
            const auto dim_y = grid.m_bounded_grid.m_dim_y;
            const auto dim_z = grid.m_bounded_grid.m_dim_z;

            return {
                static_cast<unsigned int>(idx % dim_x),
                static_cast<unsigned int>((idx / dim_x) % dim_x),
                static_cast<unsigned int>(idx / (dim_x * dim_y))
            };
        }
    );

    const bool is_using_variance = !grid.m_lengths_variance.empty();
    m_xs.resize(index3d_with_data.size());
    m_ys.resize(index3d_with_data.size());
    m_zs.resize(index3d_with_data.size());
    m_counts.resize(index3d_with_data.size());
    m_hits.resize(index3d_with_data.size());
    m_pads.resize(index3d_with_data.size());
    m_lengths.resize(index3d_with_data.size());
    m_hits_lengths.resize(index3d_with_data.size());
    if (is_using_variance)
        m_lengths_variance.resize(index3d_with_data.size());

    // Copying indexes
    const auto h5_size_t = H5::PredType::NATIVE_UINT;
    std::ranges::copy(index3d_with_data | std::views::elements<0>, m_xs.begin());
    std::ranges::copy(index3d_with_data | std::views::elements<1>, m_ys.begin());
    std::ranges::copy(index3d_with_data | std::views::elements<2>, m_zs.begin());

    std::ranges::copy(
        index_with_data | std::views::transform([&grid](const size_t& index) -> unsigned int {
            return grid.m_hits[index];
        }),
        m_hits.begin()
    );
    std::ranges::copy(
        index_with_data | std::views::transform([&grid](const size_t& index) -> unsigned int {
            return grid.m_counts[index];
        }),
        m_counts.begin()
    );

    {
        std::ranges::copy(
            index_with_data | std::views::transform([&grid](const size_t& index) -> unsigned int {
                return grid.m_lengths[index];
            }),
            m_lengths.begin()
        );
    }

    if (is_using_variance)
    {
        std::ranges::copy(
            index_with_data | std::views::transform([&grid](const size_t& index) -> unsigned int {
                return grid.m_lengths_variance[index];
            }),
            m_lengths_variance.begin()
        );
    }

    assert(
        m_xs.size() == m_ys.size() && m_ys.size() == m_counts.size() &&
        m_counts.size() == m_hits.size() && m_hits.size() == m_pads.size() &&
        m_pads.size() == m_lengths.size() && m_lengths.size() == m_hits_lengths.size()
    );
}

COOGrid::VoxelViewIterator::VoxelViewIterator()
    : m_index{}
    , m_grid{}
    , m_value{}
{
}

COOGrid::VoxelViewIterator::VoxelViewIterator(COOGrid& grid)
    : m_index{0}
    , m_grid{&grid}
    , m_value{}
{
    update_value();
}

COOGrid::VoxelViewIterator::VoxelViewIterator(const VoxelViewIterator& other)
    : m_index{other.m_index}
    , m_grid{other.m_grid}
    , m_value{other.m_value}
{
}

auto COOGrid::VoxelViewIterator::operator=(const VoxelViewIterator& other) -> VoxelViewIterator&
{
    m_index = other.m_index;
    m_grid  = other.m_grid;
    m_value = other.m_value;

    return *this;
}

auto COOGrid::VoxelViewIterator::operator+=(difference_type diff) -> VoxelViewIterator&
{
    m_index += diff;
    update_value();
    return *this;
}

auto COOGrid::VoxelViewIterator::operator-=(difference_type diff) -> VoxelViewIterator&
{
    m_index -= diff;
    update_value();
    return *this;
}

auto COOGrid::VoxelViewIterator::operator++() -> VoxelViewIterator&
{
    *this += 1;
    return *this;
}

auto COOGrid::VoxelViewIterator::operator++(int) -> VoxelViewIterator
{
    auto tmp = *this;
    ++*this;
    return tmp;
}

auto COOGrid::VoxelViewIterator::operator--() -> VoxelViewIterator&
{
    *this -= 1;
    return *this;
}

auto COOGrid::VoxelViewIterator::operator--(int) -> VoxelViewIterator
{
    auto tmp = *this;
    ++*this;
    return tmp;
}

auto COOGrid::VoxelViewIterator::operator+(difference_type diff) const -> VoxelViewIterator
{
    auto tmp = *this;
    tmp += diff;
    return tmp;
}

auto COOGrid::VoxelViewIterator::operator-(difference_type diff) const -> VoxelViewIterator
{
    auto tmp = *this;
    tmp -= diff;
    return tmp;
}

auto COOGrid::VoxelViewIterator::operator*() -> value_type
{
    return m_value;
}

auto COOGrid::VoxelViewIterator::operator->() -> pointer
{
    return &m_value;
}

auto COOGrid::VoxelViewIterator::operator==(const VoxelViewIterator& other) const -> bool
{
    return m_grid == other.m_grid && m_index == other.m_index;
}

auto COOGrid::VoxelViewIterator::operator!=(const VoxelViewIterator& other) const -> bool
{
    return !(*this == other);
}

auto COOGrid::VoxelViewIterator::operator<(const VoxelViewIterator& other) const -> bool
{
    return m_index < other.m_index;
}

auto COOGrid::VoxelViewIterator::operator>(const VoxelViewIterator& other) const -> bool
{
    return m_index > other.m_index;
}

auto COOGrid::VoxelViewIterator::operator<=(const VoxelViewIterator& other) const -> bool
{
    return (*this < other) || (*this == other);
}

auto COOGrid::VoxelViewIterator::operator>=(const VoxelViewIterator& other) const -> bool
{
    return (*this > other) || (*this == other);
}

auto COOGrid::VoxelViewIterator::operator[](size_t index) -> VoxelViewIterator
{
    auto tmp    = *this;
    tmp.m_index = index;
    tmp.update_value();
    return tmp;
}

auto COOGrid::VoxelViewIterator::update_value() -> void
{
    m_value = value_type{
        &m_grid->m_xs[m_index],
        &m_grid->m_ys[m_index],
        &m_grid->m_zs[m_index],
        &m_grid->m_counts[m_index],
        &m_grid->m_hits[m_index],
        &m_grid->m_pads[m_index],
        &m_grid->m_lengths[m_index],
        &m_grid->m_hits_lengths[m_index],
        nullptr
    };

    if (!m_grid->m_lengths_variance.empty())
    {
        m_value.length_variance = &m_grid->m_lengths_variance[m_index];
    }
}

auto COOGrid::compute_pad(algorithms::pe::BeerLambert) -> void
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

auto COOGrid::compute_pad(algorithms::pe::ContactFrequency) -> void
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

auto COOGrid::compute_pad(algorithms::pe::UnequalPathLengthBeerLambert) -> void
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

auto COOGrid::export_to_h5(
    const std::string& dataset_name, const std::filesystem::path& filename, bool include_all_data
) const -> void
{
    Logger     logger{"Grid HDF5 export"};
    H5::H5File file;
    if (std::filesystem::exists(filename))
        std::filesystem::remove(filename);

    file = H5::H5File{filename.string(), H5F_ACC_TRUNC};

    const hsize_t voxels_with_data = std::ranges::distance(m_pads);

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
        xs_data.write(m_xs.data(), h5_index_t);
    }

    {
        H5::DataSet ys_data = get_or_create_dataset("y", h5_index_t, data_space);
        ys_data.write(m_ys.data(), h5_index_t);
    }

    {
        H5::DataSet zs_data = get_or_create_dataset("z", h5_index_t, data_space);
        zs_data.write(m_zs.data(), h5_index_t);
    }

    {
        H5::PredType h5_pad_t    = H5::PredType::NATIVE_DOUBLE;
        H5::DataSet  values_data = get_or_create_dataset("pad", h5_pad_t, data_space);
        values_data.write(m_pads.data(), h5_pad_t);
    }

    if (include_all_data)
    {
        {
            H5::PredType h5_hits_t = H5::PredType::NATIVE_UINT;
            H5::DataSet  hits_data = get_or_create_dataset("hits", h5_hits_t, data_space);
            hits_data.write(m_hits.data(), h5_hits_t);
        }

        {
            H5::PredType h5_counts_t = H5::PredType::NATIVE_UINT;
            H5::DataSet  counts_data = get_or_create_dataset("counts", h5_counts_t, data_space);
            counts_data.write(m_counts.data(), h5_counts_t);
        }

        {
            H5::PredType h5_lengths_t = H5::PredType::NATIVE_DOUBLE;
            H5::DataSet  lengths_data = get_or_create_dataset("lengths", h5_lengths_t, data_space);
            lengths_data.write(m_lengths.data(), h5_lengths_t);
        }

        if (!m_lengths_variance.empty())
        {
            H5::PredType h5_lengths_var_t = H5::PredType::NATIVE_DOUBLE;
            H5::DataSet  length_variances_data =
                get_or_create_dataset("lengths variance", h5_lengths_var_t, data_space);
            length_variances_data.write(m_lengths_variance.data(), h5_lengths_var_t);
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

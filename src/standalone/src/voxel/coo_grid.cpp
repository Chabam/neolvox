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
    : m_size{}
    , m_uses_variance{grid.m_compute_variance}
    , m_xs{}
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
        all_indexes.resize(ChunkedGrid::VoxelChunk::s_cell_count);
        std::iota(all_indexes.begin(), all_indexes.end(), 0);
        auto index_with_data = all_indexes | std::views::filter([&chunk](auto idx) -> bool {
                                   return chunk->m_counts[idx] != 0;
                               });
        std::vector<Index3D> index3d_with_data;
        std::ranges::transform(
            index_with_data,
            std::back_inserter(index3d_with_data),
            [chunk_x, chunk_y, chunk_z](size_t idx) -> Index3D {
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

        std::ranges::copy(
            index_with_data | std::views::transform([&chunk](const size_t& index) -> double {
                return chunk->m_lengths[index];
            }),
            std::back_inserter(m_lengths)
        );

        std::ranges::copy(
            index_with_data | std::views::transform([&chunk](const size_t& index) -> double {
                return chunk->m_hits_lengths[index];
            }),
            std::back_inserter(m_hits_lengths)
        );

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

    m_size = m_xs.size();
    m_pads.resize(m_size);

    assert(
        m_xs.size() == m_size &&                                  //
        m_ys.size() == m_size &&                                  //
        m_zs.size() == m_size &&                                  //
        m_counts.size() == m_size &&                              //
        m_hits.size() == m_size &&                                //
        m_pads.size() == m_size &&                                //
        m_lengths.size() == m_size &&                             //
        m_hits_lengths.size() == m_size &&                        //
        (!m_uses_variance || m_lengths_variance.size() == m_size) //
    );
}

COOGrid::COOGrid(const DenseGrid& grid)
    : m_size{}
    , m_uses_variance{!grid.m_lengths_variance.empty()}
    , m_xs{}
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

    m_size = index3d_with_data.size();
    m_xs.resize(m_size);
    m_ys.resize(m_size);
    m_zs.resize(m_size);
    m_counts.resize(m_size);
    m_hits.resize(m_size);
    m_pads.resize(m_size);
    m_lengths.resize(m_size);
    m_hits_lengths.resize(m_size);
    if (m_uses_variance)
        m_lengths_variance.resize(m_size);

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

    if (m_uses_variance)
    {
        std::ranges::copy(
            index_with_data | std::views::transform([&grid](const size_t& index) -> unsigned int {
                return grid.m_lengths_variance[index];
            }),
            m_lengths_variance.begin()
        );
    }
    assert(
        m_xs.size() == m_size &&                                  //
        m_ys.size() == m_size &&                                  //
        m_zs.size() == m_size &&                                  //
        m_counts.size() == m_size &&                              //
        m_hits.size() == m_size &&                                //
        m_pads.size() == m_size &&                                //
        m_lengths.size() == m_size &&                             //
        m_hits_lengths.size() == m_size &&                        //
        (!m_uses_variance || m_lengths_variance.size() == m_size) //
    );
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

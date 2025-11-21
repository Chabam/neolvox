#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <iterator>
#include <numeric>
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
        {
            ++chunk_idx;
            continue;
        }

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
                static_cast<unsigned int>((idx / dim_x) % dim_y),
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
                auto aggregate = grid.m_lengths_variance[index].load(std::memory_order_relaxed);
                if (aggregate->m_count < 2)
                {
                    return NAN;
                }

                return aggregate->m_m2 / aggregate->m_count;
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

} // namespace lvox

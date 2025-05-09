#ifndef LVOX_GRID_TEST_FIXTURES_HPP
#define LVOX_GRID_TEST_FIXTURES_HPP

#include <gtest/gtest.h>
#include <utils/utils.hpp>

#include <lvox/voxel/grid.hpp>

#include "lvox/voxel/concrete_grid.hpp"

template <class GridT, lvox::Index x, lvox::Index y, lvox::Index z, double cell_size>
struct VoxelGridTestParam
{
    using grid_t = GridT;

    double       m_cell_size{cell_size};
    lvox::Index  m_dim_x{x};
    lvox::Index  m_dim_y{y};
    lvox::Index  m_dim_z{z};
    lvox::Bounds m_bounds{create_bounds(x, y, z)};
    GridT        m_grid{m_bounds, m_cell_size};
};

template <class GridT>
class VoxelGridTests : public testing::Test
{
  protected:
    using grid_t = GridT;
};

using DenseGridU32  = lvox::DenseGrid<std::uint32_t>;
using SparseGridU32 = lvox::SparseGrid<std::uint32_t>;
using GridTypes     = testing::Types<DenseGridU32, SparseGridU32>;

TYPED_TEST_SUITE(VoxelGridTests, GridTypes);

#endif // LVOX_GRID_TEST_FIXTURES_HPP

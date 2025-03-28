#ifndef LVOX_GRID_TEST_FIXTURES_HPP
#define LVOX_GRID_TEST_FIXTURES_HPP

#include <gtest/gtest.h>
#include <utils/utils.hpp>

#include <lvox/voxel/grid.hpp>

template <class GridT, size_t x, size_t y, size_t z, double cell_size>
struct VoxelGridTestParam
{
    using grid_t = GridT;

    double               m_cell_size{cell_size};
    size_t               m_dim_x{x};
    size_t               m_dim_y{y};
    size_t               m_dim_z{z};
    lvox::Grid::bounds_t m_bounds{create_bounds(x, y, z)};
    GridT                m_grid{m_bounds, m_cell_size};
};

template <class GridT>
class VoxelGridTests : public testing::Test
{
  protected:
      using grid_t = GridT;
};

#endif // LVOX_GRID_TEST_FIXTURES_HPP

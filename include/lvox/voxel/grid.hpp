#ifndef LVOX_VOXEL_GRID_HPP
#define LVOX_VOXEL_GRID_HPP

#include <variant>

#include <lvox/types.hpp>
#include <lvox/voxel/chunked_grid.hpp>
#include <lvox/voxel/dense_grid.hpp>

namespace lvox
{

using Grid = std::variant<DenseGrid, ChunkedGrid>;

} // namespace lvox

#endif // LVOX_VOXEL_GRID_HPP

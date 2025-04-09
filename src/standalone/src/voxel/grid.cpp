#include <format>
#include <ranges>
#include <stdexcept>
#include <vector>

#include <lvox/voxel/grid.hpp>

auto lvox::Grid::traversal(Grid& grid, const Beam& beam) -> void
{
    const Beam::vector_t beam_origin = beam.origin();
    const Grid::bounds_t grid_bounds = grid.bounds();

    [[unlikely]]
    if (!grid_bounds.contains(beam_origin.x(), beam_origin.y(), beam_origin.z()))
    {
        throw std::runtime_error{std::format(
            "Beam of origin ({},{},{}) is outside the grid", beam_origin.x(), beam_origin.y(), beam_origin.z()
        )};
    }

    const Beam::vector_t beam_direction = beam.direction();

    // Source: https://stackoverflow.com/a/4609795
    const auto get_axis_dir = [](double val) -> signed char {
        return (0. < val) - (val < 0.);
    };

    // Woo and Amanatides's fast traversal algorithm
    // Variables references:
    // X = idx_x
    // Y = idx_y
    // Z = idx_z
    // stepY, stepX, stepZ = step
    const auto [idx_x, idx_y, idx_z]        = grid.index_of_point(beam.origin());
    const Grid::bounds_t       voxel_bounds = grid.voxel_bounds(idx_x, idx_y, idx_z);
    const double               cell_size    = grid.cell_size();
    std::array<signed char, 3> step         = {
        //
        get_axis_dir(beam_direction.x()),
        get_axis_dir(beam_direction.y()),
        get_axis_dir(beam_direction.z())
    };
    /*Beam::vector_t t_max =*/
}

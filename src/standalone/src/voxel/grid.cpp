#include <array>
#include <format>
#include <stdexcept>

#include <lvox/voxel/grid.hpp>

auto lvox::Grid::traversal(Grid& grid, const Beam& beam) -> void
{
    const Beam::vector_t beam_origin = beam.origin();
    const Grid::bounds_t grid_bounds = grid.bounds();

    [[unlikely]]
    if (!grid_bounds.contains(beam_origin.x(), beam_origin.y(), beam_origin.z()))
    {
        throw std::runtime_error{std::format(
            "Beam of origin ({},{},{}) is outside the grid",
            beam_origin.x(),
            beam_origin.y(),
            beam_origin.z()
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
    const auto [idx_x, idx_y, idx_z]              = grid.index_of_point(beam.origin());
    const double                        cell_size = grid.cell_size();
    const Eigen::Vector<signed char, 3> step      = {
        get_axis_dir(beam_direction.x()),
        get_axis_dir(beam_direction.y()),
        get_axis_dir(beam_direction.z())
    };

    const auto [starting_x, starting_y, starting_z] = grid.index_of_point(beam_origin);
#ifdef GRID_TRAVERSAL_USING_VECTOR
    auto start_index =
        Eigen::Vector<size_t, 3>{starting_x, starting_y, starting_z}.array().max(1UL);

    const Eigen::Vector3d min_bounds{grid_bounds.minx, grid_bounds.minx, grid_bounds.minz};
    const Eigen::Vector3d grid_aligned_start{start_index.cast<double>() * cell_size};

    Eigen::Vector3d t_max =
        (min_bounds + grid_aligned_start - beam_origin).array() / beam_direction.array();
    Eigen::Vector3d t_delta = cell_size / (beam_direction.array() * step.array().cast<double>());
#else
    double t_max_x;
    double t_delta_x;
    if (step.x() > 0.0)
    {
        t_max_x =
            (grid_bounds.minx + std::max(starting_x, 1UL) * grid.cell_size() - beam_origin.x()) /
            beam_direction.x();
        t_delta_x = cell_size / beam_direction.x();
    }
    else if (step.x() < 0.0)
    {
        t_max_x = (grid_bounds.minx + (std::max(starting_x, 1UL) - 1) * grid.cell_size() -
                   beam_origin.x()) /
                  beam_direction.x();
        t_delta_x = cell_size / -beam_direction.x();
    }
    else
    {
        t_delta_x = 0;
    }


    double t_max_y;
    double t_delta_y;
    if (step.y() > 0.0)
    {
        t_max_y =
            (grid_bounds.miny + std::max(starting_y, 1UL) * grid.cell_size() - beam_origin.y()) /
            beam_direction.y();
        t_delta_y = cell_size / beam_direction.y();
    }
    else if (step.y() < 0.0)
    {
        t_max_y = (grid_bounds.miny + (std::max(starting_y, 1UL) - 1) * grid.cell_size() -
                   beam_origin.y()) /
                  beam_direction.y();
        t_delta_y = cell_size / -beam_direction.y();
    }
    else
    {
        t_delta_y = 0;
    }


    double t_max_z;
    double t_delta_z;
    if (step.z() > 0.0)
    {
        t_max_z =
            (grid_bounds.minx + std::max(starting_z, 1UL) * grid.cell_size() - beam_origin.z()) /
            beam_direction.z();
        t_delta_z = cell_size / beam_direction.z();
    }
    else if (step.z() < 0.0)
    {
        t_max_z = (grid_bounds.minx + (std::max(starting_z, 1UL) - 1) * grid.cell_size() -
                   beam_origin.z()) /
                  beam_direction.z();
        t_delta_z = cell_size / -beam_direction.z();
    }
    else
    {
        t_delta_z = 0;
    }
#endif

    /*Beam::vector_t t_max =*/
}

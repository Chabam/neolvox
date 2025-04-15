#include <format>
#include <stdexcept>

#include <lvox/voxel/grid.hpp>

auto lvox::Grid::traversal(const Grid& grid, const Beam& beam) -> std::vector<idxs_t>
{
    using GridIndex = Eigen::Vector<size_t, 3>;
    using Step      = Eigen::Vector<signed char, 3>;

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
    const auto [idx_x, idx_y, idx_z] = grid.index_of_point(beam.origin());
    const double cell_size           = grid.cell_size();

    const Step step = {
        get_axis_dir(beam_direction.x()),
        get_axis_dir(beam_direction.y()),
        get_axis_dir(beam_direction.z())
    };

    const idxs_t current_voxel = grid.index_of_point(beam_origin);

    // NOTE: replacing zeros the minimal double value to avoid dividing by zero.
    // This way, when we'll compute the delta and tmax, we'll get a very high value instead of an
    // error. This **should** work out just fine for the algorithm.
    // TODO: It doesn't seem to be working correctly. The direction can sometimes be
    // very big and sometimes VERY small. Making some case VERY long to compute. Disabling it for
    // now, might investigate later.
    //
    // const Beam::vector_t corrected_dir =
    //     beam_direction.array().max(std::numeric_limits<double>::min());

    // NOTE: In the case of negative direction, the next voxel boundary is not "above" but below. In
    // that case we must correct the boundary accordingly. See:
    // https://github.com/francisengelmann/fast_voxel_traversal/issues/7#issue-374675911
    const Beam::vector_t negative_correction = step.unaryExpr([cell_size](signed char val) {
        return val < 0. ? cell_size : 0.;
    });

    auto [current_voxel_x, current_voxel_y, current_voxel_z] = current_voxel;
    const Beam::vector_t next_bound =
        GridIndex{current_voxel_x, current_voxel_y, current_voxel_z}.array().cast<double>() +
        step.array().cast<double>() * cell_size + negative_correction.array();

    Beam::vector_t       t_max = (next_bound - beam_origin).array() / beam_direction.array();
    const Beam::vector_t delta = cell_size / beam_direction.array() * step.array().cast<double>();

    std::vector<idxs_t> visited_voxels;
    visited_voxels.reserve((Beam::vector_t{grid_bounds.minx, grid_bounds.miny, grid_bounds.minz} -
                            Beam::vector_t{grid_bounds.maxx, grid_bounds.maxy, grid_bounds.maxz})
                               .norm());

    const size_t dim_x = grid.dim_x();
    const size_t dim_y = grid.dim_y();
    const size_t dim_z = grid.dim_z();

    while (true)
    {
        visited_voxels.push_back({current_voxel_x, current_voxel_y, current_voxel_z});
        if (t_max.x() < t_max.y())
        {
            if (t_max.x() < t_max.z())
            {
                t_max.x() += delta.x();
                current_voxel_x += step.x();
            }
            else
            {
                t_max.z() += delta.z();
                current_voxel_z += step.z();
            }
        }
        else
        {
            if (t_max.y() < t_max.z())
            {
                t_max.y() += delta.y();
                current_voxel_y += step.y();
            }
            else
            {
                t_max.z() += delta.z();
                current_voxel_z += step.z();
            }
        }

        // Going out of bounds
        if (current_voxel_x < 0 || current_voxel_y < 0 || current_voxel_z < 0 ||
            current_voxel_x > dim_x || current_voxel_y > dim_y || current_voxel_z > dim_z)
        {
            break;
        }
    }

    return visited_voxels;
}

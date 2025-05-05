#include <format>
#include <stdexcept>

#include <lvox/logger/logger.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{

constexpr auto g_grid_traversal_info = R"(
Beam
    beam_origin    ({}, {}, {})
    direction ({}, {}, {})
t_max ({}, {}, {})
Delta ({}, {}, {})
)";

auto Grid::traversal(const Grid& grid, const Beam& beam, const double max_distance)
    -> std::vector<idxs_t>
{

    Logger logger{"Grid traversal"};

    using GridIndex      = Eigen::Vector<size_t, 3>;
    using Step           = Eigen::Vector<signed char, 3>;
    using vector_t       = Beam::vector_t;
    using bounds_t       = Grid::bounds_t;
    constexpr double inf = std::numeric_limits<double>::infinity();

    const bounds_t bounds    = grid.bounds();
    const size_t   dim_x     = grid.dim_x();
    const size_t   dim_y     = grid.dim_y();
    const size_t   dim_z     = grid.dim_z();
    const double   cell_size = grid.cell_size();

    const vector_t beam_origin    = beam.origin();
    const vector_t beam_direction = beam.direction();

    [[unlikely]]
    if (!bounds.contains(beam_origin.x(), beam_origin.y(), beam_origin.z()))
    {
        throw std::runtime_error{std::format(
            "Beam of beam_origin ({},{},{}) is outside the grid",
            beam_origin.x(),
            beam_origin.y(),
            beam_origin.z()
        )};
    }

    // Source: https://stackoverflow.com/a/4609795
    // Returns -1 if the slope is negative, 1 if positive and 0 if there's no slope.
    const auto get_axis_dir = [](double val) -> signed char {
        return (0. < val) - (val < 0.);
    };

    const Step step = beam_direction.unaryExpr(get_axis_dir);

    // NOTE: replacing zeros the minimal double value to avoid dividing by zero.
    // This way, when we'll compute the delta and t_max, we'll get a very high value instead of an
    // error. This **should** work out just fine for the algorithm.
    // TODO: It doesn't seem to be working correctly. The direction can sometimes be
    // very big and sometimes VERY small. Making some case VERY long to compute. Disabling it for
    // now, might investigate later.
    //
    // const vector_t corrected_dir =
    //     beam_direction.array().max(std::numeric_limits<double>::min());

    // NOTE: In the case of negative direction, the next voxel boundary is not "above" but below. In
    // that case we must correct the boundary accordingly. See:
    // https://github.com/francisengelmann/fast_voxel_traversal/issues/7#issue-374675911
    const vector_t negative_correction = step.unaryExpr([cell_size](signed char val) {
        return val < 0. ? cell_size : 0.;
    });

    auto [current_voxel_x, current_voxel_y, current_voxel_z] = grid.index_of_point(beam_origin);

    const bounds_t voxel_bounds =
        grid.voxel_bounds(current_voxel_x, current_voxel_y, current_voxel_z);

    const vector_t inv_dir = 1. / beam_direction.array();

    vector_t t_max;
    vector_t delta;
    // X()
    if (beam_direction.x() > 0)
    {
        t_max.x() = (voxel_bounds.maxx - beam_origin.x()) * inv_dir.x();
        delta.x() = cell_size * inv_dir.x();
    }
    else if (beam_direction.x() < 0)
    {
        t_max.x() = (voxel_bounds.minx - beam_origin.x()) * inv_dir.x();
        delta.x() = -cell_size * inv_dir.x();
    }
    else
    {
        t_max.x() = inf;
        delta.x() = inf;
    }

    // Y
    if (beam_direction.y() > 0)
    {
        t_max.y() = (voxel_bounds.maxy - beam_origin.y()) * inv_dir.y();
        delta.y() = cell_size * inv_dir.y();
    }
    else if (beam_direction.y() < 0)
    {
        t_max.y() = (voxel_bounds.miny - beam_origin.y()) * inv_dir.y();
        delta.y() = -cell_size * inv_dir.y();
    }
    else
    {
        t_max.y() = inf;
        delta.y() = inf;
    }

    // Z
    if (beam_direction.z() > 0)
    {
        t_max.z() = (voxel_bounds.maxz - beam_origin.z()) * inv_dir.z();
        delta.z() = cell_size * inv_dir.z();
    }
    else if (beam_direction.z() < 0)
    {
        t_max.z() = (voxel_bounds.minz - beam_origin.z()) * inv_dir.z();
        delta.z() = -cell_size * inv_dir.z();
    }
    else
    {
        t_max.z() = inf;
        delta.z() = inf;
    }

    std::vector<idxs_t> visited_voxels;

    // Reserve two times the longest line in the grid bounds. This estimate should be a best effort
    // to reduce the memory allocation time.
    const vector_t min_pts{bounds.minx, bounds.miny, bounds.minz};
    const vector_t max_pts{bounds.maxx, bounds.maxy, bounds.maxz};
    visited_voxels.reserve(((max_pts - min_pts).norm() / cell_size) * 2);

    logger.debug(
        g_grid_traversal_info,
        beam_origin.x(),
        beam_origin.y(),
        beam_origin.z(),
        beam_direction.x(),
        beam_direction.y(),
        beam_direction.z(),
        t_max.x(),
        t_max.y(),
        t_max.z(),
        delta.x(),
        delta.y(),
        delta.z()
    );

    double distance = 0.;
    do
    {
        visited_voxels.push_back({current_voxel_x, current_voxel_y, current_voxel_z});
        if (t_max.x() < t_max.y())
        {
            if (t_max.x() < t_max.z())
            {
                distance = t_max.x();
                t_max.x() += delta.x();
                current_voxel_x += step.x();
            }
            else
            {
                distance = t_max.z();
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

                distance = t_max.z();
                t_max.z() += delta.z();
                current_voxel_z += step.z();
            }
        }
    } while (current_voxel_x >= 0 && current_voxel_y >= 0 && current_voxel_z >= 0 &&
             current_voxel_x <= dim_x && current_voxel_y <= dim_y && current_voxel_z <= dim_z &&
             distance <= max_distance);

    visited_voxels.shrink_to_fit();

    return visited_voxels;
}

} // namespace lvox

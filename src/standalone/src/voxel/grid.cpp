#include <format>
#include <stdexcept>

#include <lvox/logger/logger.hpp>
#include <lvox/scanner/beam.hpp>
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

auto Grid::traversal(
    const Grid&                                        grid,
    const Beam&                                        beam,
    const std::function<void(const Index3D&, double)>& callback,
    const double                                       max_distance
) -> void
{

    Logger logger{"Grid traversal"};

    using GridIndex      = Eigen::Vector<Index, 3>;
    using Step           = Eigen::Vector<signed char, 3>;
    constexpr double inf = std::numeric_limits<double>::infinity();

    const Bounds bounds    = grid.bounds();
    const Index dim_x     = grid.dim_x();
    const Index dim_y     = grid.dim_y();
    const Index dim_z     = grid.dim_z();
    const double cell_size = grid.cell_size();

    const Point  beam_origin    = beam.origin();
    const Vector beam_direction = beam.direction();

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

    auto [current_voxel_x, current_voxel_y, current_voxel_z] = grid.index_of_point(beam_origin);

    const Bounds voxel_bounds =
        grid.voxel_bounds(current_voxel_x, current_voxel_y, current_voxel_z);

    const Vector inv_dir = 1. / beam_direction.array();

    const auto compute_t_max_component = //
        [inf](
            double dir_component,
            double origin_component,
            double min_bound,
            double max_bound,
            double inv_dir_component
        ) {
            if (dir_component > 0.)
                return (max_bound - origin_component) * inv_dir_component;
            else if (dir_component < 0.)
                return (min_bound - origin_component) * inv_dir_component;
            else
                return inf;
        };

    Vector t_max{
        compute_t_max_component(
            beam_direction.x(), beam_origin.x(), voxel_bounds.minx, voxel_bounds.maxx, inv_dir.x()
        ),
        compute_t_max_component(
            beam_direction.y(), beam_origin.y(), voxel_bounds.miny, voxel_bounds.maxy, inv_dir.y()
        ),
        compute_t_max_component(
            beam_direction.z(), beam_origin.z(), voxel_bounds.minz, voxel_bounds.maxz, inv_dir.z()
        )
    };
    const Vector delta = (cell_size * step.unaryExpr([](const double val) -> double {
                             return val == 0. ? inf : val;
                         })).array() *
                         inv_dir.array();

    logger.verbose(
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
        callback({current_voxel_x, current_voxel_y, current_voxel_z}, distance);
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
             current_voxel_x < dim_x && current_voxel_y < dim_y && current_voxel_z < dim_z &&
             distance <= max_distance);
}

} // namespace lvox

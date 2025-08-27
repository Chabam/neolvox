#ifndef LVOX_GRID_TRAVERSAL
#define LVOX_GRID_TRAVERSAL

#include <format>
#include <limits>
#include <stdexcept>

#include <lvox/logger/logger.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{

namespace algorithms
{

constexpr auto g_grid_traversal_info = R"(
Beam
    beam_origin    ({}, {}, {})
    direction ({}, {}, {})
t_max ({}, {}, {})
Delta ({}, {}, {})
)";

struct VoxelHitInfo
{
    Index3D m_index;
    double  m_distance_in_voxel;
};

template <bool exact_distance>
struct GridTraversal
{
    GridTraversal(const Grid& grid) : m_grid{grid} {};

    template <typename HitCallback>
    auto operator()(
        const Beam& beam,
        HitCallback&& callback,
        const double  max_distance = std::numeric_limits<double>::infinity()
    ) -> void
    {

        Logger logger{"Grid traversal"};

        using GridIndex      = Eigen::Vector<unsigned int, 3>;
        using Step           = Eigen::Vector<signed char, 3>;
        constexpr double inf = std::numeric_limits<double>::infinity();

        const Bounds bounds    = m_grid.bounds();
        const size_t  dim_x     = m_grid.dim_x();
        const size_t  dim_y     = m_grid.dim_y();
        const size_t  dim_z     = m_grid.dim_z();
        const double cell_size = m_grid.cell_size();

        const Point  beam_origin    = beam.origin();
        const Vector beam_direction = beam.direction();

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
        static const auto get_axis_dir = [](double val) -> signed char {
            return (0. < val) - (val < 0.);
        };

        const Step step = beam_direction.unaryExpr(get_axis_dir);

        auto [current_voxel_x, current_voxel_y, current_voxel_z] = m_grid.index3d_of_point(beam_origin);

        const Bounds voxel_bounds =
            m_grid.voxel_bounds(current_voxel_x, current_voxel_y, current_voxel_z);

        const Vector inv_dir = 1. / beam_direction.array();

        static const auto compute_t_max_component = //
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
                beam_direction.x(),
                beam_origin.x(),
                voxel_bounds.minx,
                voxel_bounds.maxx,
                inv_dir.x()
            ),
            compute_t_max_component(
                beam_direction.y(),
                beam_origin.y(),
                voxel_bounds.miny,
                voxel_bounds.maxy,
                inv_dir.y()
            ),
            compute_t_max_component(
                beam_direction.z(),
                beam_origin.z(),
                voxel_bounds.minz,
                voxel_bounds.maxz,
                inv_dir.z()
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

        double       total_traveled_distance = 0.;
        VoxelHitInfo current_hit{
            .m_index             = Index3D{current_voxel_x, current_voxel_y, current_voxel_z},
            .m_distance_in_voxel = 0.,
        };
        bool can_continue = true;
        do
        {
            Index3D prev_idx      = current_hit.m_index;
            double  prev_distance = total_traveled_distance;

            if (t_max.x() < t_max.y())
            {
                if (t_max.x() < t_max.z())
                {
                    total_traveled_distance = t_max.x();
                    t_max.x() += delta.x();
                    current_voxel_x += step.x();
                }
                else
                {
                    total_traveled_distance = t_max.z();
                    t_max.z() += delta.z();
                    current_voxel_z += step.z();
                }
            }
            else
            {
                if (t_max.y() < t_max.z())
                {
                    total_traveled_distance = t_max.y();
                    t_max.y() += delta.y();
                    current_voxel_y += step.y();
                }
                else
                {

                    total_traveled_distance = t_max.z();
                    t_max.z() += delta.z();
                    current_voxel_z += step.z();
                }
            }

            current_hit.m_distance_in_voxel = total_traveled_distance - prev_distance;

            const bool is_above_min_bounds =
                current_voxel_x >= 0 && current_voxel_y >= 0 && current_voxel_z >= 0;
            const bool is_under_max_bounds =
                current_voxel_x < dim_x && current_voxel_y < dim_y && current_voxel_z < dim_z;
            const bool is_under_max_distance = total_traveled_distance <= max_distance;

            can_continue = is_above_min_bounds && is_under_max_bounds && is_under_max_distance;

            if constexpr (exact_distance)
            {
                if (prev_distance + current_hit.m_distance_in_voxel > max_distance)
                    current_hit.m_distance_in_voxel = max_distance - prev_distance;
            }

            if (current_hit.m_distance_in_voxel > 0)
                callback(current_hit);

            current_hit.m_index = Index3D{current_voxel_x, current_voxel_y, current_voxel_z};
        } while (can_continue);
    }

    const Grid& m_grid;
};

using GridTraversalVoxelRounding = GridTraversal<false>;
using GridTraversalExactDistance = GridTraversal<true>;

} // namespace algorithms

} // namespace lvox

#endif // LVOX_GRID_TRAVERSAL

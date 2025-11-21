#include <lvox/algorithms/compute_scene_bounds.hpp>
#include <lvox/scanner/scan.hpp>

namespace lvox::algorithms
{

lvox::Bounds compute_scene_bounds(const std::vector<lvox::Scan>& scans)
{
    Bounds total_bounds;

    for (const auto& scan : scans)
        total_bounds.grow(scan.m_bounds);

    return total_bounds;
}
} // namespace lvox::algorithms

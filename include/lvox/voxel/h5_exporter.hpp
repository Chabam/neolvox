#ifndef LVOX_H5_EXPORTER_HPP
#define LVOX_H5_EXPORTER_HPP

#include <filesystem>

#include <lvox/voxel/grid.hpp>

namespace lvox
{

namespace h5_exporter
{

auto export_grid(
    const lvox::Grid& result, const std::string& dataset, const std::filesystem::path& file
) -> void;

// auto export_grid(
//     const lvox::Grid& result, const std::string& dataset, const std::filesystem::path& file
// ) -> void;

} // namespace h5_exporter

} // namespace lvox

#endif // LVOX_H5_EXPORTER_HPP

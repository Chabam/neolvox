#ifndef LVOX_TEST_GRID_HELPERS
#define LVOX_TEST_GRID_HELPERS

#include <pdal/PDALUtils.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/util/Bounds.hpp>

// Creates a bound from -(dim/2) to (dim/2)
auto create_bounds(double dim_x, double dim_y, double dim_z) -> pdal::BOX3D;

// Generates a point cloud of 8 points at the corners of a cube from -1 to 1
auto generate_cubic_point_cloud(pdal::PointTableRef table) -> pdal::PointViewPtr;

#endif // !LVOX_TEST_GRID_HELPERS

#ifndef LVOX_TEST_GRID_HELPERS
#define LVOX_TEST_GRID_HELPERS

#include <pdal/PDALUtils.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/util/Bounds.hpp>

auto get_current_time_as_gps_time() -> double;

struct Point
{
    Point(double x, double y, double z, double gps_time = get_current_time_as_gps_time());

    double x;
    double y;
    double z;
    double gps_time;
};

// Creates a bound from -(dim/2) to (dim/2)
auto create_bounds(double dim_x, double dim_y, double dim_z) -> lvox::Bounds;

// Generates a point cloud of 8 points at the corners of a cube from -1 to 1
auto generate_cubic_point_cloud(
    pdal::PointTableRef table, double dim_x = 2., double dim_y = 2., double dim_z = 2.
) -> pdal::PointViewPtr;

// Generates a point cloud of N points all located within the given cubic bounds.
auto generate_cubic_point_cloud_with_random_points(
    pdal::PointTableRef table, size_t point_count, double dim_x = 2., double dim_y = 2., double dim_z = 2.
) -> pdal::PointViewPtr;


#endif // !LVOX_TEST_GRID_HELPERS

#ifndef LVOX_TEST_GRID_HELPERS
#define LVOX_TEST_GRID_HELPERS

#include <pdal/PDALUtils.hpp>
#include <pdal/PointTable.hpp>

#include <lvox/voxel/bounds.hpp>

double get_current_time_as_gps_time();

struct Point
{
    Point(double x, double y, double z, double gps_time = get_current_time_as_gps_time());

    double x() const { return m_x; }
    double y() const { return m_y; }
    double z() const { return m_z; }
    double gps_time() const { return m_gps_time; }

    double m_x;
    double m_y;
    double m_z;
    double m_gps_time;
};

using PointCloud = std::vector<Point>;

// Creates a bound from -(dim/2) to (dim/2)
lvox::Bounds create_bounds(double dim_x, double dim_y, double dim_z);

// Generates a point cloud of 8 points at the corners of a cube from -1 to 1
PointCloud generate_cubic_point_cloud(
    double dim_x = 2., double dim_y = 2., double dim_z = 2.
);

// Generates a point cloud of N points all located within the given cubic bounds.
PointCloud generate_cubic_point_cloud_with_random_points(
    size_t              point_count,
    double              dim_x = 2.,
    double              dim_y = 2.,
    double              dim_z = 2.
);

#endif // !LVOX_TEST_GRID_HELPERS

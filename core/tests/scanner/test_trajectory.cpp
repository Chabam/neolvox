#include <gtest/gtest.h>
#include <utils/utils.hpp>

#include <lvox/types.hpp>
#include <lvox/scanner/trajectory.hpp>

lvox::Trajectory<Point, PointCloud> get_trajectory() {
    // Intentionnally shuffling these points
    static const PointCloud trajectory_pc{
        Point{0, 0, 1, 10},
        Point{0, 0, 7, 60},
        Point{0, 0, 5, 40},
        Point{0, 0, 3, 20},
        Point{0, 0, 4, 30},
        Point{0, 0, 2, 10},
        Point{0, 0, 6, 50}
    };

    return lvox::Trajectory<Point, PointCloud>{trajectory_pc};
}


TEST(TrajectoryTests, before_first)
{
    const auto traj = get_trajectory();
    const auto pt = traj.interpolate_point_from_gps_time(0);

    EXPECT_EQ(1, pt.z());
}


TEST(TrajectoryTests, after_last)
{
    const auto traj = get_trajectory();
    const auto pt = traj.interpolate_point_from_gps_time(100);

    EXPECT_EQ(7, pt.z());
}



TEST(TrajectoryTests, interpolate)
{
    const auto traj = get_trajectory();
    const auto pt = traj.interpolate_point_from_gps_time(55);

    EXPECT_EQ((6+7)/2., pt.z());
}

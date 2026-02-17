#include <chrono>
#include <random>
#include <ranges>
#include <utils/utils.hpp>

#include <lvox/types.hpp>
#include <lvox/voxel/bounds.hpp>

double get_current_time_as_gps_time()
{
    namespace c = std::chrono;
    const auto current_time = c::system_clock::now().time_since_epoch();
    return static_cast<double>(c::duration_cast<c::microseconds>(current_time).count());
}

Point::Point(double x, double y, double z, double gps_time)
    : m_x{x}
    , m_y{y}
    , m_z{z}
    , m_gps_time{gps_time}
{
}


lvox::Bounds<double> create_bounds(double dim_x, double dim_y, double dim_z)
{
    const double half_dim_x = dim_x / 2.0;
    const double half_dim_y = dim_y / 2.0;
    const double half_dim_z = dim_z / 2.0;

    return lvox::Bounds<double>{-half_dim_x, half_dim_x, -half_dim_y, half_dim_y, -half_dim_z, half_dim_z};
}

PointCloud generate_cubic_point_cloud(double dim_x, double dim_y, double dim_z)
{
    const double half_dim_x = dim_x / 2.0;
    const double half_dim_y = dim_y / 2.0;
    const double half_dim_z = dim_z / 2.0;
    return {
        // Top square
        Point{-half_dim_x, -half_dim_y, half_dim_z},
        Point{-half_dim_x, half_dim_y, half_dim_z},
        Point{half_dim_x, half_dim_y, half_dim_z},
        Point{half_dim_x, -half_dim_y, half_dim_z},

        // Bottom
        Point{-half_dim_x, -half_dim_y, -half_dim_z},
        Point{-half_dim_x, half_dim_y, -half_dim_z},
        Point{half_dim_x, half_dim_y, -half_dim_z},
        Point{half_dim_x, -half_dim_y, -half_dim_z},
    };
}

PointCloud generate_cubic_point_cloud_with_random_points(
    size_t point_count, double dim_x, double dim_y, double dim_z
)
{
    const double half_dim_x = dim_x / 2.0;
    const double half_dim_y = dim_y / 2.0;
    const double half_dim_z = dim_z / 2.0;

    // Fixing the seed for reproducibility. Maybe not that useful?
    std::mt19937                           gen{1337};
    std::uniform_real_distribution<double> dis_x(-half_dim_x, half_dim_x);
    std::uniform_real_distribution<double> dis_y(-half_dim_y, half_dim_y);
    std::uniform_real_distribution<double> dis_z(-half_dim_z, half_dim_z);

    PointCloud pc;

    for (size_t i = 0; i < point_count; ++i)
    {
        pc.emplace_back(dis_x(gen), dis_y(gen), dis_z(gen));
    }

    return pc;
}

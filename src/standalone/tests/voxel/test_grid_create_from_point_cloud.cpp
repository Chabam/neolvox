#include <voxel/test_grid_fixtures.hpp>
#include <pdal/PointView.hpp>


TYPED_TEST(VoxelGridTests, create_from_point_cloud)
{
    pdal::PointTable table;
    const auto       view = generate_cubic_point_cloud(table);

    lvox::Bounds point_cloud_bounds;
    const double         cell_size = 0.5;
    view->calculateBounds(point_cloud_bounds);
    typename TestFixture::grid_t grid{point_cloud_bounds, cell_size};

    for (const auto pt : *view)
    {
        const double x = pt.getFieldAs<double>(pdal::Dimension::Id::X);
        const double y = pt.getFieldAs<double>(pdal::Dimension::Id::Y);
        const double z = pt.getFieldAs<double>(pdal::Dimension::Id::Z);
        ASSERT_TRUE(grid.bounds().contains(x, y, z));
    }
}

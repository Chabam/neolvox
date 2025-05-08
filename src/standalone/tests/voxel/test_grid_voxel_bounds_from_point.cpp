#include <voxel/test_grid_fixtures.hpp>

TYPED_TEST(VoxelGridTests, voxel_bounds_from_point)
{
    // Very concious choice of an odd number
    const double dim = 9;

    lvox::Bounds bounds = create_bounds(dim, dim, dim);

    const double                 cell_size = 1.;
    typename TestFixture::grid_t grid{bounds, cell_size};

    const double       middle_point = (dim / 2.);
    const lvox::Bounds middle_voxel_from_dim =
        grid.voxel_bounds(middle_point, middle_point, middle_point);
    const lvox::Bounds middle_voxel_from_point =
        grid.voxel_bounds_from_point(lvox::Vector::Constant(0.));

    EXPECT_EQ(middle_voxel_from_dim, middle_voxel_from_point);
}

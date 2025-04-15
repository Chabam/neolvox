#include <voxel/test_grid_fixtures.hpp>

TYPED_TEST(VoxelGridTests, voxel_bounds_from_point)
{
    // Very concious choice of an odd number
    const double dim = 9;

    lvox::Grid::bounds_t bounds = create_bounds(dim, dim, dim);

    const double                 cell_size = 1.;
    typename TestFixture::grid_t grid{bounds, cell_size};

    const double               middle_point = (dim / 2.) + 1;
    const lvox::Grid::bounds_t middle_voxel_from_dim =
        grid.voxel_bounds(middle_point, middle_point, middle_point);
    const lvox::Grid::bounds_t middle_voxel_from_point =
        grid.voxel_bounds_from_point(Eigen::Vector3d::Constant(0.));

    EXPECT_EQ(middle_voxel_from_dim, middle_voxel_from_point);
}

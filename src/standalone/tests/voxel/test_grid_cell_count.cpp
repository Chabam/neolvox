#include <voxel/test_grid_fixtures.hpp>


TYPED_TEST(VoxelGridTests, cell_count)
{
    using grid_t           = TestFixture::grid_t;
    const double cell_size = 1.;
    const double dim_x     = 10;
    const double dim_y     = 20;
    const double dim_z     = 30;

    lvox::Bounds bounds = create_bounds(dim_x, dim_y, dim_z);

    typename TestFixture::grid_t grid{bounds, cell_size};
    ASSERT_EQ(dim_x * dim_y * dim_z, grid.cell_count());
}

# LIBLVOX Developer Documentation

## Compilation 

Compile using Qt 5.15. The library will be in the `release` folder. By default compiles as a **static** library. 
Comment with a `#` the line `CONFIG += staticlib`.

There are no dependencies to Qt libraries, only the compilation toolchain is used. Only one external library is used, Eigen which you can either specify
manually in the `INCLUDEPATH` variable or by installing it with VCPKG and setting a `VCPHG` environement variable containing this path : `<vcpkg_root>/installed/<system_triplet>/include`

To deploy the lib, in the Qt creator project configuration go to `Projects > Build > Build steps > Add build step "Make"` and set argument to `install`.

## Developement

The library is built around 4 algorithms and a few datastructures. 
This section will detail what should be implemented to implement LVOX in your project.

### Datastructures 

The datastructures are the basic building blocs of this library. The implementation is heavily inspired by the Computree 
implementation.

#### Grid3d 

The grid is the base bloc of the algorithm. The implementation should look something like that :
```
template<classname T>
class YourGridType : public lvox::Grid3d<T>
{
public:
/**
    Implement all virtual methods
*/

};
```
At that point, you have a choice. Either your grid type already take into account the existence of the distance grids as 
it is the case in Computree's implementation or not, and you have to provide the distance grids. Either way, make them a 
private member of your class and pass them as-is when liblvox requires a grid type.

On a technical level, the grid should be **axis-aligned**. The grid's **resolution** is the size of a an individual voxel 
in *meters*. The **dimension<x,y,z>** is the number of voxels on each axis. **Lambda1** is a value required to compute the 
PAD. As it is computed in an algorithm before it is used in another, the user has the responsibility of storing it and 
passing it around.

#### Shooting Pattern

The internal representation of a LIDAR scanner. 

#### Point Cloud Scene 

The scanner point cloud.

### Algorithms

The four algorithms are described below, as well as how to use them. 
All the memory in the algorithms are handled by the user, as such, no pointers passed in are deleted.

#### ComputeLvoxGrids

To compute the grids, you first need a shooting pattern and a point cloud. The association is as follow :
```
struct ItemPreparator
{
    PointCloudScene* PointCloudScene;   ///< Reference to the PointCloudScene.
    ShootingPattern* ShootingPattern;   ///< Reference to the ShootingPattern.
};
```
For each preparator, you need a point cloud and a shooting pattern. You also need a variety of settings :
```
struct Settings
{
    GridMode GridMode;                  ///< The grid computation mode.
    Eigen::Vector3d Dimension;          ///< The dimension of the grid.
    Eigen::Vector3d Position;           ///< The position of the grid.
    double GridResolution;              ///< The resolution of the grid.
    bool MustComputeDistances;          ///< Flag to indicate whether distances need to be computed.
    VegetationType VegetationType;      ///< The vegetation type if MustComputeDistances is True. Undefined when flag is false
    union {
        struct {
            double Length;
            double Diameter;
        };///< This is for NeedleFromDimension
        struct {
            double Extension1;
            double Extension2;
        }; ///< This is for BroadLeafFromDimension
        double FlatProjectedArea; ///< This is for NeedleFromFlatArea && BroadLeafFromFlatArea
    } VegetationSettings;
};
```
The GridMode specify of, if need be, should the grid size be computed. You can specify the position as well as the extent of the grid on each axes.
If you want to compute the PAD in the next algorithm, you need to set the `MustComputDistances` flag to true and set vegetation element informations.

You can now get the computed bounding bow to set your grids.

Finally, it's time to compute the grids. Pass in a collection of compute items :
```
struct ComputeItem
{
    PointCloudScene* PointCloudScene;   ///< Pointer to the PointCloudScene.
    ShootingPattern* ShootingPattern;   ///< Pointer to the ShootingPattern.
    Grid3d<int32_t>* HitsGrid;          ///< Pointer to the hits grid.
    Grid3d<int32_t>* TheoreticalGrid;   ///< Pointer to the theoretical grid.
    Grid3d<int32_t>* BeforeGrid;        ///< Pointer to the before grid.
};
```
and call : `algo.compute(computeItemsVector);`.

**Note on performance** : There are lots of indirections due to the nature of the library. You should pay close attention to the
way you retreive the 3D points in the point cloud as well as the rays in the shooting pattern. The more performant those two parts are,
the faster the algorithm will run.

#### ComputePAD

#### InterpolateDistances

#### MergeGrids

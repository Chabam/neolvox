#ifndef COMPUTELVOXGRIDS_H
#define COMPUTELVOXGRIDS_H

#include <Eigen/Geometry>
#include <vector>

#include "../datastructures/grid3d.h"
#include "../datastructures/pointcloudscene.h"
#include "../datastructures/shootingpattern.h"
#include "../tools/logger.h"
#include "../workers/manager.h"

namespace lvox
{
/**
 * @class ComputeLvoxGrids
 * @brief A class to compute lvox grids based on specified settings and data.
 *
 * This class provides functionality to compute lvox grids based on user-defined
 * settings and input data. It supports different grid modes and allows computation
 * of grids with various configurations.
 */
class __declspec(dllexport) ComputeLvoxGrids
{
public:
    /**
     * @enum GridMode
     * @brief Enumerates different modes for grid computation.
     *
     * This enumeration defines various modes for computing lvox grids, each
     * mode represents a different configuration for grid computation.
     */
    enum class GridMode
    {
        BoundingBoxOfTheScene = 0,                    ///< Compute grid based on the bounding box of the scene.
        RelativeToCoordinates,                        ///< Compute grid relative to specified coordinates.
        RelativeToCoordinatesAndCustomDimensions,     ///< Compute grid with custom dimensions relative to specified coordinates.
        CenteredOnCoordinatesAndCustomDimensions,     ///< Compute grid centered on specified coordinates with custom dimensions.
        FromGridFileParameters,                       ///< Compute grid based on parameters from a grid file.
        FromOtherGrid                                 ///< Compute grid based on another grid.
    };

    /**
     * @enum VegetationType
     * @brief Enumerates the different vegetation types used to compute Lambda1
     *
     */
    enum VegetationType {
        NeedleFromDimension = 0,
        BroadLeafFromDimension,
        NeedleFromFlatArea,
        BroadLeafFromFlatArea
    };

    /**
     * @struct Settings
     * @brief Represents the settings for lvox grid computation.
     *
     * This structure holds various settings required for computing lvox grids.
     */
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

    /**
     * @struct ComputeItem
     * @brief Represents the data needed for grid computation.
     *
     * This structure holds the required data and references for computing the grids.
     */
    struct ComputeItem
    {
        PointCloudScene* PointCloudScene;   ///< Pointer to the PointCloudScene.
        ShootingPattern* ShootingPattern;   ///< Pointer to the ShootingPattern.
        Grid3d<int32_t>* HitsGrid;          ///< Pointer to the hits grid.
        Grid3d<int32_t>* TheoreticalGrid;   ///< Pointer to the theoretical grid.
        Grid3d<int32_t>* BeforeGrid;        ///< Pointer to the before grid.
    };

    struct ItemPreparator
    {
        PointCloudScene* PointCloudScene;   ///< Reference to the PointCloudScene.
        ShootingPattern* ShootingPattern;   ///< Reference to the ShootingPattern.
    };

    /**
     * @brief Constructor to create a ComputeLvoxGrids object.
     * @param computeItems Vector of data from which to compute the grids.
     * @param settings Parameters from which the computation will be derived.
     */
//    ComputeLvoxGrids(std::vector<ComputeItem>& computeItems, Settings& settings);

    /**
     * @brief Constructor to create a ComputeLvoxGrids object.
     * @param computeItems Vector of data from which to compute the grids.
     * @param settings Parameters from which the computation will be derived.
     */
    ComputeLvoxGrids(std::vector<ItemPreparator>& computeItems, Settings& settings, Logger* log = nullptr);

    /**
     * @brief Compute lvox grids based on the specified settings and data.
     */
    void compute(std::vector<ComputeItem>& items);

    bool getInitBoundingBox(Eigen::Vector3d& minBox, Eigen::Vector3d& maxBox) const;

private:
    double computeElementProjectedArea();


    Settings& _settings;                 ///< Reference to the settings for grid computation.
    std::vector<ItemPreparator>& _computeItems;///< Vector of ComputeItem objects containing data for grid computation.
    lvox::Logger* _logger;
    Eigen::AlignedBox3d _voxelBox;
};

} // namespace lvox

#endif // COMPUTELVOXGRIDS_H

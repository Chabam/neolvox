/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_COMPUTELVOXGRIDSPREPARATOR_H
#define LVOX3_COMPUTELVOXGRIDSPREPARATOR_H

#include <QString>
#include <QHash>

#include "lvox3_gridmode.h"
#include "ct_step/abstract/ct_abstractstep.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_shootingpatternd.h"


#include "Eigen/Core"

class CT_AbstractItemGroup;
class CT_ShootingPattern;
class CT_AbstractImage2D;
class CT_ResultGroupIterator;
class CT_VirtualAbstractStep;

/**
 * @brief Use this class to prepare all elements to compute lvox grids
 */
class LVOX3_ComputeLVOXGridsPreparator
{
public:
    struct ToCompute {
        CT_Scene*               scene;
        CT_ShootingPattern*     pattern;
    };

    struct Result {
        typedef QHash<CT_StandardItemGroup*, ToCompute >            ToComputeCollection;
        typedef QHashIterator<CT_StandardItemGroup*, ToCompute >    ToComputeCollectionIterator;

        Result() { valid = false; }

        bool                valid;
        ToComputeCollection elementsToCompute;
        Eigen::Vector3d     minBBox;
        Eigen::Vector3d     maxBBox;
    };

    struct Coordinates {
        Eigen::Vector3d coordinate;
        Eigen::Vector3d dimension;
    };

    LVOX3_ComputeLVOXGridsPreparator();

    Result prepare(const CT_VirtualAbstractStep* step,
                   const CT_HandleInStdGroup<>& inGroup,
                   const CT_HandleInResultGroupCopy<>& inResult,
                   const CT_HandleInSingularItem<CT_Scanner>& inScan,
                   const CT_HandleInSingularItem<CT_ShootingPatternD>& inShootingPattern,
                   double gridResolutionx,
                   double gridResolutiony,
                   double gridResolutionz,
                   lvox::GridMode gridMode,
                   const QString& gridFilePath = "");
    /**
     * @brief Call it to prepare elements to compute lvox grids
     * @param step : your step (this)
     * @param itGrp : iterator on group that contains the scene, scanner or shooting pattern
     * @param sceneModelName : scene model name to find it
     * @param scannerModelName : scanner model name to find it
     * @param shotPatternModelName : shooting pattern model name to find it
     * @param mntModelName : mnt model name to find it
     * @param skyModelName : sky model name to find it
     * @param gridResolutionx : the grid resolution on the x axis (for one cell)
     * @param gridResolutiony : the grid resolution on the y axis (for one cell)
     * @param gridResolutionz : the grid resolution on the z axis (for one cell)
     * @param gridMode : grid mode to use
     * @param coord : coordinate information if gridMode == ...Coordinates...
     * @param gridFilePath : filepath to .grid file (onlyt if gridMode == FromGridFileParameters)
     * @return result of the preparation
     */

    Result prepare(const CT_VirtualAbstractStep* step,
                  const CT_HandleInStdGroup<>& inGroup,
                  const CT_HandleInResultGroupCopy<>& inResult,
                  const CT_HandleInSingularItem<CT_AbstractItemDrawableWithPointCloud>& inScene,
                  const CT_HandleInSingularItem<CT_Scanner>& inScan,
                  const CT_HandleInSingularItem<CT_ShootingPatternD>& inShootingPattern,
                  double gridResolutionX,
                  double gridResolutionY,
                  double gridResolutionZ,
                  lvox::GridMode gridMode,
                  Coordinates coord,
                  const QString& gridFilePath);
};

#endif // LVOX3_COMPUTELVOXGRIDSPREPARATOR_H

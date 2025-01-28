
#include "lvox3_computelvoxgridspreparator.h"

//Drawables
#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_shootingpatternd.h"

#include "ct_log/ct_logmanager.h"

//Class called in step compute lvox grids to prepare the bounding box and the differente elements of the scene
LVOX3_ComputeLVOXGridsPreparator::LVOX3_ComputeLVOXGridsPreparator()
{

}

LVOX3_ComputeLVOXGridsPreparator::Result LVOX3_ComputeLVOXGridsPreparator::prepare(const CT_VirtualAbstractStep* step,
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
                                                                                   const QString& gridFilePath)
{
    Q_UNUSED(step)
    Q_UNUSED(inShootingPattern)
    Result res;
    Eigen::AlignedBox3d sceneBox;
    Eigen::AlignedBox3d scannerBox;
    Eigen::AlignedBox3d voxelBox;
    std::vector<Eigen::Vector3d> positions;

    for (CT_StandardItemGroup* group : inGroup.iterateOutputs(inResult))
    {
        qDebug() << "Group ID " << group->id();
        const CT_Scene* scene = (const CT_Scene*) group->singularItem(inScene);
        const CT_Scanner* scanner = (const CT_Scanner*) group->singularItem(inScan);
        const CT_ShootingPattern* pattern = scanner->shootingPattern();

        positions.push_back(scanner->position());

        //qDebug() << "Prepare shooting pattern " << pattern->id() << " scene " << scene->id() << " pattern shots" << pattern->getNumberOfShots();

        if (scene && scanner)
        {
            ToCompute tc;
            tc.scene = const_cast<CT_Scene*>(scene);
            tc.pattern = const_cast<CT_ShootingPattern*>(pattern);

            res.elementsToCompute.insert(group, tc);

            /* Union of all scanners
             *
             * FIXME: center coordinate may not be enough to compute the
             * bounding box of the parallel shooting pattern.
             */
            const Eigen::Vector3d& scanPos = tc.pattern->centerCoordinate();
            scannerBox.extend(scanPos);

            qDebug() << "ID " << scanner->id() << " Pos : ("  << scanPos.x() << "," << scanPos.y() << "," << scanPos.z() << ")";

            /* Union of all scenes */
            Eigen::Vector3d sceneBBOXMin, sceneBBOXMax;
            scene->boundingBox(sceneBBOXMin, sceneBBOXMax);
            sceneBox.extend(sceneBBOXMin);
            sceneBox.extend(sceneBBOXMax);
        }

    }
    PS_LOG->addInfoMessage(LogInterface::trace, "Mode " + lvox::gridModeToString(gridMode));

    //Grid mode, default or custom mode
    switch(gridMode) {
    case lvox::BoundingBoxOfTheScene:
        voxelBox.extend(sceneBox);
        break;
    case lvox::RelativeToCoordinates:
        voxelBox.extend(coord.coordinate);
        voxelBox.extend(sceneBox.max());
        break;
    case lvox::RelativeToCoordinatesAndCustomDimensions:
    case lvox::FromOtherGrid:
        // custom bounding box, where useless voxels are removed
        voxelBox.extend(coord.coordinate);
        voxelBox.extend(coord.coordinate + coord.dimension);
        voxelBox.clamp(sceneBox);
        break;
    case lvox::CenteredOnCoordinatesAndCustomDimensions:
        voxelBox.extend(coord.coordinate - coord.dimension);
        voxelBox.extend(coord.coordinate + coord.dimension);
        break;
    case lvox::FromGridFileParameters:
        Q_UNUSED(gridFilePath);
        PS_LOG->addInfoMessage(LogInterface::step, "LArchitect bounding box not supported");
        return res;
        break;
    default:
        break;
    }

    // extends the bounding box to include the scanner
    if (!voxelBox.contains(scannerBox)) {
        PS_LOG->addMessage(LogInterface::warning, LogInterface::step,
                           QObject::tr("Grid dimensions has been enlarged to include scanner and/or their range"));
        //TEMPORARY FIX BY FP 11/2017 TO AVOID THAT THE GRID CENTER IS SHIFTED WHEN SCANNER ARE NOT IN THE GRID
        if (gridMode==lvox::CenteredOnCoordinatesAndCustomDimensions) {
            Eigen::Vector3d v1s = scannerBox.min().array();
            Eigen::Vector3d v2s = scannerBox.max().array();
            Eigen::Vector3d v1 = voxelBox.min().array();
            Eigen::Vector3d v2 = voxelBox.max().array();
            while(v1(0)>v1s(0)) {v1(0) -= gridResolutionX;}
            while(v1(1)>v1s(1)) {v1(1) -= gridResolutionY;}
            while(v1(2)>v1s(2)) {v1(2) -= gridResolutionZ;}
            while(v2(0)<v2s(0)) {v2(0) += gridResolutionX;}
            while(v2(1)<v2s(1)) {v2(1) += gridResolutionY;}
            while(v2(2)<v2s(2)) {v2(2) += gridResolutionZ;}
            voxelBox.extend(v1);
            voxelBox.extend(v2);
        } else {
            voxelBox.extend(scannerBox);
        }
    }

    //setting response dimensions
    res.minBBox = voxelBox.min();
    res.maxBBox = voxelBox.max();

    PS_LOG->addMessage(LogInterface::info, LogInterface::step,
                       QObject::tr("Voxel bounding box: (%1,%2,%3), (%4,%5,%6)")
                        .arg(res.minBBox.x(), 5, 'f', 1)
                        .arg(res.minBBox.y(), 5, 'f', 1)
                        .arg(res.minBBox.z(), 5, 'f', 1)
                        .arg(res.maxBBox.x(), 5, 'f', 1)
                        .arg(res.maxBBox.y(), 5, 'f', 1)
                        .arg(res.maxBBox.z(), 5, 'f', 1)
                       );

    res.valid = true;
    return res;
}

/****************************************************************************

 Copyright (C) 2012-2012 Universite de Sherbrooke, Quebec, CANADA
                     All rights reserved.

 Contact :  richard.fournier@usherbrooke.ca
            jean-francois.cote@nrcan-rncan.gc.ca
            joris.ravaglia@gmail.com

 Developers : Joris RAVAGLIA
 Adapted by Alexandre Piboule for Computree 2.0

 This file is part of Computree version 2.0.

 Computree is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Computree is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Computree.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#include "lvox2_stepcomputelvoxgrids.h"

#include "ct_result/model/inModel/ct_inresultmodelgrouptocopy.h"
#include "ct_result/model/outModel/tools/ct_outresultmodelgrouptocopypossibilities.h"

// Inclusion of standard result class
#include "ct_result/ct_resultgroup.h"

// Inclusion of used ItemDrawable classes
#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_itemdrawable/ct_pointsattributesscalartemplated.h"
#include "qvector3d.h"

#include "ct_step/abstract/ct_abstractsteploadfile.h"
#include "ct_view/ct_stepconfigurabledialog.h"

#include "tools/lvox_computehitsthread.h"
#include "tools/lvox_computetheoriticalsthread.h"
#include "tools/lvox_computebeforethread.h"
#include "tools/lvox_computedensitythread.h"
#include "urfm/tools/lvox2_computeactualbeamthread.h"
#include "urfm/tools/lvox2_computehitsthread.h"
#include "ct_iterator/ct_pointiterator.h"

#include <QFileInfo>
#include <QDebug>
#include <limits>
#include <stdlib.h>

#define DEF_SearchInResult "r"
#define DEF_SearchInScene   "sc"
#define DEF_SearchInFlag   "flag"
#define DEF_SearchInScan   "sca"
#define DEF_SearchInGroup   "gr"

LVOX2_StepComputeLvoxGrids::LVOX2_StepComputeLvoxGrids(CT_StepInitializeData &dataInit) : CT_AbstractStep(dataInit)
{
    //********************************************//
    //              Attributes of LVox            //
    //********************************************//
    _res = 0.1; //0.5;
    _effectiveRayThresh = 0;
    _computeDistances = false;
    _ntMode = 1; //0; // by default, Beam number is theoretical
    _gridMode = 3;//1
    _xBase = 0;//-20.0;
    _yBase = 0;//-20.0;
    _zBase = 0;//-10.0;

    _xDim = 60;//80;
    _yDim = 60;//80;
    _zDim = 40;//80;
    _cylindricFilter = true;
}

QString LVOX2_StepComputeLvoxGrids::getStepDescription() const
{
    // Gives the descrption to print in the GUI
    return tr("2- Calculer les grilles LVOX");
}

CT_VirtualAbstractStep* LVOX2_StepComputeLvoxGrids::createNewInstance(CT_StepInitializeData &dataInit)
{
    // Creates an instance of this step
    return new LVOX2_StepComputeLvoxGrids(dataInit);
}

void LVOX2_StepComputeLvoxGrids::createInResultModelListProtected()
{
    CT_InResultModelGroupToCopy *resultModel = createNewInResultModelForCopy(DEF_SearchInResult, tr("Scène(s)"));

    resultModel->setZeroOrMoreRootGroup();
    resultModel->addGroupModel("", DEF_SearchInGroup, CT_AbstractItemGroup::staticGetType(), tr("Scan"));
    resultModel->addItemModel(DEF_SearchInGroup, DEF_SearchInScene, CT_Scene::staticGetType(), tr("Scène"));
    resultModel->addItemModel(DEF_SearchInGroup, DEF_SearchInScan, CT_Scanner::staticGetType(), tr("Scanner"));
    resultModel->addItemModel(DEF_SearchInGroup, DEF_SearchInFlag, CT_PointsAttributesScalarTemplated<int>::staticGetType(),tr("Flag"), "", CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);

}

void LVOX2_StepComputeLvoxGrids::createOutResultModelListProtected()
{

    // create a new OUT result that is a copy of the IN result selected by the user
    CT_OutResultModelGroupToCopyPossibilities *res = createNewOutResultModelToCopy(DEF_SearchInResult);

    if (res != NULL)
    {
        // in this result we add a CT_AffiliationID to the group named DEF_SearchInGroup. The name of the model of
        // the CT_AffiliationID will be generated automatically by the _outAffiliationIDModelName object.
        res->addItemModel(DEF_SearchInGroup, _hits_ModelName, new CT_Grid3D<int>(), tr("Hits"));
        res->addItemAttributeModel(_hits_ModelName, _NiFlag_ModelName, new CT_StdItemAttributeT<bool>("LVOX_GRD_NI"), tr("isNi"));

        if (_ntMode==0||_ntMode==2) // Nt is theoritical (initial formulation)
        {
            res->addItemModel(DEF_SearchInGroup, _theo_ModelName, new CT_Grid3D<int>(), tr("Theoretical"));
            res->addItemAttributeModel(_theo_ModelName, _NtFlag_ModelName, new CT_StdItemAttributeT<bool>("LVOX_GRD_NT"), tr("isNt"));
        }
        if (_ntMode==1||_ntMode==2) {// Nt is actual
            res->addItemModel(DEF_SearchInGroup, _actu_ModelName, new CT_Grid3D<int>(), tr("ActualTotal"));
            res->addItemAttributeModel(_actu_ModelName, _NtaFlag_ModelName, new CT_StdItemAttributeT<bool>("LVOX_GRD_NTA"), tr("isNta"));
        }
        res->addItemModel(DEF_SearchInGroup, _bef_ModelName, new CT_Grid3D<int>(), tr("Before"));
        res->addItemAttributeModel(_bef_ModelName, _NbFlag_ModelName, new CT_StdItemAttributeT<bool>("LVOX_GRD_NB"), tr("isNb"));

        res->addItemModel(DEF_SearchInGroup, _density_ModelName, new CT_Grid3D<float>(), tr("Density"));
        res->addItemAttributeModel(_density_ModelName, _DensityFlag_ModelName, new CT_StdItemAttributeT<bool>("LVOX_GRD_DENSITY"), tr("isDensity"));


        if (_computeDistances)
        {
            res->addItemModel(DEF_SearchInGroup, _deltain_ModelName, new CT_Grid3D<float>(), tr("DeltaIn"));
            res->addItemModel(DEF_SearchInGroup, _deltaout_ModelName, new CT_Grid3D<float>(), tr("DeltaOut"));
            if (_ntMode==0||_ntMode==2) // Nt is theoritical (initial formulation)
            {
                res->addItemModel(DEF_SearchInGroup, _deltatheo_ModelName, new CT_Grid3D<float>(), tr("DeltaTheoretical"));

            }
            if (_ntMode==1||_ntMode==2) { // Nt is actual
                res->addItemModel(DEF_SearchInGroup, _deltaactu_ModelName, new CT_Grid3D<float>(), tr("DeltaActualTotal"));
            }
            res->addItemModel(DEF_SearchInGroup, _deltabef_ModelName, new CT_Grid3D<float>(), tr("DeltaBefore"));

        }
    }

}

void LVOX2_StepComputeLvoxGrids::createPostConfigurationDialog()
{
    CT_StepConfigurableDialog *configDialog = newStandardPostConfigurationDialog();

    //********************************************//
    //              Attributes of LVox            //
    //********************************************//
    configDialog->addDouble(tr("Resolution of the grids"),tr("meters"),0.0001,10000,2, _res );
    configDialog->addInt(tr("Minimum number of effective ray in a voxel to take it into account"),tr(""),0,100000, _effectiveRayThresh );
    configDialog->addBool(tr("Compute Distances"), tr(""), tr(""), _computeDistances);
    //configDialog->addBool(tr("Compute actual beams"), tr(""), tr(""), _computeActualBeams);
    configDialog->addEmpty();
    configDialog->addText(tr("Computation of total beam number :"),"", "");
    CT_ButtonGroup &bg_ntMode = configDialog->addButtonGroup(_ntMode);
    configDialog->addExcludeValue("", "", tr("Default mode : Theoretical number"), bg_ntMode, 0);
    configDialog->addExcludeValue("", "", tr("Actual beam number (a full point cloud is required)"), bg_ntMode, 1);
    configDialog->addExcludeValue("", "", tr("Both are computed"), bg_ntMode, 2);

    configDialog->addEmpty();

    configDialog->addText(tr("Reference for (minX, minY, minZ) corner of the grid :"),"", "");

    CT_ButtonGroup &bg_gridMode = configDialog->addButtonGroup(_gridMode);
    configDialog->addExcludeValue("", "", tr("Default mode : Bounding box of the scene"), bg_gridMode, 0);
    configDialog->addExcludeValue("", "", tr("Custom mode : Relative to following coordinates:"), bg_gridMode, 1);
    configDialog->addExcludeValue("", "", tr("Custom mode : Relative to following coordinates + custom dimensions:"), bg_gridMode, 2);
    configDialog->addExcludeValue("", "", tr("Custom mode : centered on following coordinates + custom dimensions:"), bg_gridMode, 3);


    configDialog->addDouble(tr("X coordinate:"), "", -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 4, _xBase);
    configDialog->addDouble(tr("Y coordinate:"), "", -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 4, _yBase);
    configDialog->addDouble(tr("Z coordinate:"), "", -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 4, _zBase);

    configDialog->addInt(tr("X dimension:"), "", 1, 1000, _xDim);
    configDialog->addInt(tr("Y dimension:"), "", 1, 1000, _yDim);
    configDialog->addInt(tr("Z dimension:"), "", 1, 1000, _zDim);

   configDialog->addBool(tr("Placette cylindrique"),"", "", _cylindricFilter);
}

void LVOX2_StepComputeLvoxGrids::compute()
{   
    // Gets the out result
    CT_ResultGroup* outResult = getOutResultList().first();

    QList<CT_MonitoredQThread*> baseThreads;
    QList<CT_MonitoredQThread*> densityThreads;

    QMap<CT_AbstractItemGroup*, QPair<const CT_Scene*, const CT_Scanner*> > pointsOfView;

    // Global limits of generated grids
    double xMin = std::numeric_limits<double>::max();
    double yMin = std::numeric_limits<double>::max();
    double zMin = std::numeric_limits<double>::max();
    double xMax = -std::numeric_limits<double>::max();
    double yMax = -std::numeric_limits<double>::max();
    double zMax = -std::numeric_limits<double>::max();
    // Global limit of the enlarge grid (necessary for computations)
    double xMin2 = 0.f;
    double yMin2 = 0.f;
    double zMin2 = 0.f;
    double xMax2 = 0.f;
    double yMax2 = 0.f;
    double zMax2 = 0.f;

    double xMinScene = std::numeric_limits<double>::max();
    double yMinScene = std::numeric_limits<double>::max();
    double zMinScene = std::numeric_limits<double>::max();
    double xMaxScene = -std::numeric_limits<double>::max();
    double yMaxScene = -std::numeric_limits<double>::max();
    double zMaxScene = -std::numeric_limits<double>::max();

    double xMinScanner = std::numeric_limits<double>::max();
    double yMinScanner = std::numeric_limits<double>::max();
    double zMinScanner = std::numeric_limits<double>::max();
    double xMaxScanner = -std::numeric_limits<double>::max();
    double yMaxScanner = -std::numeric_limits<double>::max();
    double zMaxScanner = -std::numeric_limits<double>::max();



    // on va rechercher tous les groupes contenant des nuages de points (qui ont été choisi par l'utilisateur)
    CT_ResultGroupIterator itGrp(outResult, this, DEF_SearchInGroup);
    while (itGrp.hasNext() && !isStopped())
    {
        CT_AbstractItemGroup *group = (CT_AbstractItemGroup*) itGrp.next();

        const CT_Scene* scene = (CT_Scene*)group->firstItemByINModelName(this, DEF_SearchInScene);
        const CT_Scanner* scanner = (CT_Scanner*)group->firstItemByINModelName(this, DEF_SearchInScan);
        const CT_PointsAttributesScalarTemplated<int>* flag = (CT_PointsAttributesScalarTemplated<int>*)group->firstItemByINModelName(this, DEF_SearchInFlag);

        if (flag == NULL) {qDebug() << "Pas de Flag";}

        if (scene!=NULL && scanner!=NULL)
        {
            pointsOfView.insert(group, QPair<const CT_Scene*, const CT_Scanner*>(scene, scanner));

            Eigen::Vector3d min, max;
            scene->getBoundingBox(min, max);

            if (min.x() < xMinScene) {xMinScene = min.x();}
            if (min.y() < yMinScene) {yMinScene = min.y();}
            if (min.z() < zMinScene) {zMinScene = min.z();}
            if (max.x() > xMaxScene) {xMaxScene = max.x();}
            if (max.y() > yMaxScene) {yMaxScene = max.y();}
            if (max.z() > zMaxScene) {zMaxScene = max.z();}

            if (scanner->getCenterX() < xMinScanner) {xMinScanner = scanner->getCenterX();}
            if (scanner->getCenterY() < yMinScanner) {yMinScanner = scanner->getCenterY();}
            if (scanner->getCenterZ() < zMinScanner) {zMinScanner = scanner->getCenterZ();}

            if (scanner->getCenterX() > xMaxScanner) {xMaxScanner = scanner->getCenterX();}
            if (scanner->getCenterY() > yMaxScanner) {yMaxScanner = scanner->getCenterY();}
            if (scanner->getCenterZ() > zMaxScanner) {zMaxScanner = scanner->getCenterZ();}
        }
    }

    if (_gridMode == 0) {

        xMin = std::min(xMinScene, xMinScanner);
        yMin = std::min(yMinScene, yMinScanner);
        zMin = std::min(zMinScene, zMinScanner);

        xMax = std::max(xMaxScene, xMaxScanner);
        yMax = std::max(yMaxScene, yMaxScanner);
        zMax = std::max(zMaxScene, zMaxScanner);

    } else if (_gridMode == 1) {

        double xMinAdjusted = std::min(xMinScene, xMinScanner);
        double yMinAdjusted = std::min(yMinScene, yMinScanner);
        double zMinAdjusted = std::min(zMinScene, zMinScanner);

        double xMaxAdjusted = std::max(xMaxScene, xMaxScanner);
        double yMaxAdjusted = std::max(yMaxScene, yMaxScanner);
        double zMaxAdjusted = std::max(zMaxScene, zMaxScanner);

        xMin = _xBase;
        yMin = _yBase;
        zMin = _zBase;

        while (xMin < xMinAdjusted) {xMin += _res;}
        while (yMin < yMinAdjusted) {yMin += _res;}
        while (zMin < zMinAdjusted) {zMin += _res;}

        while (xMin > xMinAdjusted) {xMin -= _res;}
        while (yMin > yMinAdjusted) {yMin -= _res;}
        while (zMin > zMinAdjusted) {zMin -= _res;}

        xMax = xMin + _res;
        yMax = yMin + _res;
        zMax = zMin + _res;

        while (xMax < xMaxAdjusted) {xMax += _res;}
        while (yMax < yMaxAdjusted) {yMax += _res;}
        while (zMax < zMaxAdjusted) {zMax += _res;}

    } else if (_gridMode == 2) {

        xMin = _xBase;
        yMin = _yBase;
        zMin = _zBase;

        xMax = xMin + _res*_xDim;
        yMax = yMin + _res*_yDim;
        zMax = zMin + _res*_zDim;


    } else {

        xMin = _xBase - _res*_xDim;
        yMin = _yBase - _res*_yDim;
        zMin = _zBase - _res*_zDim;

        xMax = _xBase + _res*_xDim;
        yMax = _yBase + _res*_yDim;
        zMax = _zBase + _res*_zDim;

    }
// enlarge the grid when necessary to include scan positions
    bool enlarged = false;
    xMin2 = xMin;
    xMax2 = xMax;
    yMin2 = yMin,
    yMax2 = yMax;
    zMin2 = zMin;
    zMax2 = zMax;

    while (xMin2 > xMinScanner) {xMin2 -= _res; enlarged = true;}
    while (yMin2 > yMinScanner) {yMin2 -= _res; enlarged = true;}
    while (zMin2 > zMinScanner) {zMin2 -= _res; enlarged = true;}

    while (xMax2 < xMaxScanner) {xMax2 += _res; enlarged = true;}
    while (yMax2 < yMaxScanner) {yMax2 += _res; enlarged = true;}
    while (zMax2 < zMaxScanner) {zMax2 += _res; enlarged = true;}

    if (enlarged)
    {
        PS_LOG->addMessage(LogInterface::warning, LogInterface::step, tr("Dimensions spécifiées ne contenant pas les positions de scans : la grille a du être élargie !"));
    }
    xMin2 -= _res;
    yMin2 -= _res;
    zMin2 -= _res;
    xMax2 += _res;
    yMax2 += _res;
    zMax2 += _res;

    QMapIterator<CT_AbstractItemGroup*, QPair<const CT_Scene*, const CT_Scanner*> > it(pointsOfView);
    while (it.hasNext() && !isStopped())
    {
        it.next();
        CT_AbstractItemGroup* group = it.key();
        const CT_Scene* scene = it.value().first;
        const CT_Scanner* scanner =it.value().second;
        // check properties of the point cloud
        size_t zeros = 0;
        size_t fars = 0;
        const CT_AbstractPointCloudIndex *pointCloudIndex = scene->getPointCloudIndex();
        size_t n_points = pointCloudIndex->size();
        Eigen::Vector3d scanPos = scanner->getPosition();
        CT_PointIterator itP(pointCloudIndex);
        while (itP.hasNext())
        {
            const CT_Point &point = itP.next().currentPoint();
            if ((abs(point(0)-scanPos(0))<=0.001)&&(abs(point(1)-scanPos(1))<=0.001)&&(abs(point(2)-scanPos(2))<=0.001)) {
                zeros++;
                if (zeros==1) {
                    qDebug() << "point example" << point(0) << point(1) << point(2);
                }
            }
            if (pow(point(0)-scanPos(0),2.0)+pow(point(1)-scanPos(1),2.0)+pow(point(2)-scanPos(2),2.0)>400) {
                fars++;

            }
        }
        qDebug() << "scan: null beams="<<zeros<< "over" << n_points << "points";
         qDebug() << "scan: far beams="<<fars<< "over" << n_points << "points";

        // Declaring the output grids
        CT_Grid3D<int>*      hitGrid = CT_Grid3D<int>::createGrid3DFromXYZCoords(_hits_ModelName.completeName(), outResult, xMin2, yMin2, zMin2, xMax2, yMax2, zMax2, _res, -1, 0, true);
        hitGrid->addItemAttribute(new CT_StdItemAttributeT<bool>(_NiFlag_ModelName.completeName(), "LVOX_GRD_NI", outResult, true));
        CT_Grid3D<int>*      theoriticalGrid = NULL;
        if (_ntMode==0||_ntMode==2) // Nt is theoritical (initial formulation)
        {
            theoriticalGrid = new CT_Grid3D<int>(_theo_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);
            theoriticalGrid->addItemAttribute(new CT_StdItemAttributeT<bool>(_NtFlag_ModelName.completeName(), "LVOX_GRD_NT", outResult, true));
        }
        CT_Grid3D<int>*      actualGrid = NULL;
        if (_ntMode==1||_ntMode==2) // Nt is actual
        {
            actualGrid = new CT_Grid3D<int>(_actu_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);
            actualGrid->addItemAttribute(new CT_StdItemAttributeT<bool>(_NtaFlag_ModelName.completeName(), "LVOX_GRD_NTA", outResult, true));
        }
        CT_Grid3D<int>*      beforeGrid = new CT_Grid3D<int>(_bef_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);
        beforeGrid->addItemAttribute(new CT_StdItemAttributeT<bool>(_NbFlag_ModelName.completeName(), "LVOX_GRD_NB", outResult, true));
        CT_Grid3D<float>*   density = new CT_Grid3D<float>(_density_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);
        density->addItemAttribute(new CT_StdItemAttributeT<bool>(_DensityFlag_ModelName.completeName(), "LVOX_GRD_DENSITY", outResult, true));


        CT_Grid3D<float>*   deltaInGrid = NULL;
        CT_Grid3D<float>*   deltaOutGrid = NULL;
        CT_Grid3D<float>*   deltaTheoritical = NULL;
        CT_Grid3D<float>*   deltaActual = NULL;
        CT_Grid3D<float>*   deltaBefore = NULL;

        // grids are added to results
        group->addItemDrawable(hitGrid);
        if (_ntMode==0||_ntMode==2) { // Nt is theoretical
            group->addItemDrawable(theoriticalGrid);
        }
        if (_ntMode==1||_ntMode==2) { // Nt is actual
            group->addItemDrawable(actualGrid);
        }
        group->addItemDrawable(beforeGrid);
        group->addItemDrawable(density);

        if (_computeDistances)
        {
            deltaInGrid = new CT_Grid3D<float>(_deltain_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);
            deltaOutGrid = new CT_Grid3D<float>(_deltaout_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);
            if (_ntMode==0||_ntMode==2) {// Nt is theoretical
            deltaTheoritical = new CT_Grid3D<float>(_deltatheo_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);
            }
            if (_ntMode==1||_ntMode==2) {// Nt is actual
            deltaActual = new CT_Grid3D<float>(_deltaactu_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);
            }
            deltaBefore = new CT_Grid3D<float>(_deltabef_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);

            group->addItemDrawable(deltaInGrid);
            group->addItemDrawable(deltaOutGrid);
            if (_ntMode==0||_ntMode==2) { // Nt is theoretical
              group->addItemDrawable(deltaTheoritical);}
            if (_ntMode==1||_ntMode==2) { // Nt is actual
              group->addItemDrawable(deltaActual);}
            group->addItemDrawable(deltaBefore);
        }

        LVOX2_ComputeHitsThread* hitsThread = new LVOX2_ComputeHitsThread(scanner, hitGrid, deltaInGrid, deltaOutGrid, scene, _computeDistances, _cylindricFilter, xMin, xMax, yMin, yMax, zMin,zMax);
            connect(hitsThread, SIGNAL(progressChanged()), this, SLOT(updateProgress()));
        _threadList.append(hitsThread);
        baseThreads.append(hitsThread);

        LVOX_ComputeBeforeThread* beforeThread = new LVOX_ComputeBeforeThread(scanner, beforeGrid, deltaBefore, scene, _computeDistances);
            connect(beforeThread, SIGNAL(progressChanged()), this, SLOT(updateProgress()));
        _threadList.append(beforeThread);
        baseThreads.append(beforeThread);

        if (_ntMode==0) // Nt is theoritical (initial formulation)
        {
            LVOX_ComputeTheoriticalsThread* theoreticalThread = new LVOX_ComputeTheoriticalsThread(scanner, theoriticalGrid, deltaTheoritical, _computeDistances);
              connect(theoreticalThread, SIGNAL(progressChanged()), this, SLOT(updateProgress()));
            _threadList.append(theoreticalThread);
            baseThreads.append(theoreticalThread);

            LVOX_ComputeDensityThread* densityThread = new LVOX_ComputeDensityThread(density, hitGrid, theoriticalGrid, beforeGrid, _effectiveRayThresh);
                connect(densityThread, SIGNAL(progressChanged()), this, SLOT(updateProgress()));
            _threadList.append(densityThread);
              densityThreads.append(densityThread);
        } else if (_ntMode>=1) { // Nt is really computed (new formulation that requires that the scene contains the whole point cloud)
            LVOX2_ComputeActualBeamThread* actualBeamThread = new LVOX2_ComputeActualBeamThread(scanner, actualGrid, deltaActual, scene, _computeDistances);
            connect(actualBeamThread, SIGNAL(progressChanged()), this, SLOT(updateProgress()));
            _threadList.append(actualBeamThread);
            baseThreads.append(actualBeamThread);

            LVOX_ComputeDensityThread* densityThread = new LVOX_ComputeDensityThread(density, hitGrid, actualGrid, beforeGrid, _effectiveRayThresh);
                connect(densityThread, SIGNAL(progressChanged()), this, SLOT(updateProgress()));
            _threadList.append(densityThread);
              densityThreads.append(densityThread);

        }
        if (_ntMode==2) // Nt is theoritical (initial formulation)
        {
            LVOX_ComputeTheoriticalsThread* theoreticalThread = new LVOX_ComputeTheoriticalsThread(scanner, theoriticalGrid, deltaTheoritical, _computeDistances);
               connect(theoreticalThread, SIGNAL(progressChanged()), this, SLOT(updateProgress()));
            _threadList.append(theoreticalThread);
            baseThreads.append(theoreticalThread);
        }




    }

    int size = baseThreads.size();

    for (int i = 0 ; i < size ; ++i)
    {
        baseThreads.at(i)->start();
    }

    for (int i = 0 ; i < size ; ++i)
    {
        baseThreads.at(i)->wait();
        updateProgress();
    }

    for (int i = 0 ; i < size ; ++i)
    {
        disconnect(baseThreads.at(i), SIGNAL(progressChanged()), this, SLOT(updateProgress()));
    }

    size = densityThreads.size();

    for (int i = 0 ; i < size ; ++i)
    {
        densityThreads.at(i)->start();

    }

    for (int i = 0 ; i < size ; ++i)
    {
        densityThreads.at(i)->wait();
        updateProgress();
    }

    for (int i = 0 ; i < size ; ++i)
    {
        disconnect(densityThreads.at(i), SIGNAL(progressChanged()), this, SLOT(updateProgress()));
    }

    qDeleteAll(_threadList);



    setProgress(100);
}

void LVOX2_StepComputeLvoxGrids::updateProgress()
{
    int progress = 0;

    int size = _threadList.size();
    for (int i = 0 ; i < size ; ++i)
    {
        progress += _threadList.at(i)->getProgress();
    }

    progress /= size;
    setProgress(progress);
}


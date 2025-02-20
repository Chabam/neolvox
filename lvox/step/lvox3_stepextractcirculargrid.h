#ifndef LVOX3_STEPEXTRACTCIRCULARGRID_H
#define LVOX3_STEPEXTRACTCIRCULARGRID_H

#include "ct_step/abstract/ct_abstractstep.h"

#include "tools/3dgrid/lvox_grid3d.h"
#include "tools/lvox3_gridtype.h"

//This step extracts a circular(ish) grid for any 3dgrid.
//To note that it tests each edge of the voxel and if any of them is in the radius of the center, it is added to the result grid.

class LVOX3_StepExtractCircularGrid: public CT_AbstractStep
{
    Q_OBJECT
public:
    LVOX3_StepExtractCircularGrid();

    QString description() const;

    QString detailledDescription() const;

    CT_VirtualAbstractStep* createNewInstance() const final;

    QString outputDescription() const final;

protected:

    void declareInputModels(CT_StepInModelStructureManager& manager) final;

    void fillPostInputConfigurationDialog(CT_StepConfigurableDialog* configDialog) final;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    void compute() final;

private:
    double _x;                            /*!< Coordonnee X du centre de la grille a extraire*/
    double _y;                            /*!< Coordonnee Y du centre de la grille a extraire*/
    double _radiusmin;                    /*!< Rayon (m) de debut de la grille a extraire*/
    double _radius;                       /*!< Rayon (m) de la grille a extraire*/
    double _zmin;                         /*!< Z minimum de la grille a extraire*/
    double _zmax;                         /*!< Z maximum de la grille a extraire*/
    bool   _isSquare;


    CT_HandleInResultGroupCopy<>                        _inResult;
    CT_HandleInStdZeroOrMoreGroup                       _inZeroOrMoreRootGroup;
    CT_HandleInStdGroup<>                               _inGroup;
    CT_HandleInSingularItem<CT_AbstractGrid3D >      _inGrid;

    CT_HandleOutSingularItem<lvox::Grid3Df>             _outGrid;


    //Test to see if any part of the voxel is inside the radius of the extracted grid (If it is, it is added to the extracted grid)
    bool evaluateVoxel(Eigen::Vector3d centerCoords, double gridResolutionX, double gridResolutionY);
};

#endif // LVOX3_STEPEXTRACTCIRCULARGRID_H

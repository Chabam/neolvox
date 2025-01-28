#ifndef LVOX3_STEPGRIDNORMALISATIONRASTER_H
#define LVOX3_STEPGRIDNORMALISATIONRASTER_H

#include "ct_step/abstract/ct_abstractstep.h"

#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_image2d.h"
#include "tools/lvox3_gridtype.h"

class LVOX3_StepGridNormalisationRaster : public CT_AbstractStep
{
    Q_OBJECT
public:

    LVOX3_StepGridNormalisationRaster();

    QString description() const;

    QString detailledDescription() const;

    CT_VirtualAbstractStep* createNewInstance() const final;

    QString outputDescription() const final;

protected:

    void declareInputModels(CT_StepInModelStructureManager& manager) final;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    void compute() final;

private:
    //QHash<QString, SumType> m_SumTypeCollection;
    //QHash<QString, GridType> _GridTypeCollection;
    QString _GridType;
    QString m_SumType;

    CT_HandleInResultGroupCopy<>                        _inResult;
    CT_HandleInStdZeroOrMoreGroup                       _inZeroOrMoreRootGroup;
    CT_HandleInStdGroup<>                               _inGroup;
    CT_HandleInSingularItem<LVOX3_AbstractGrid3D >      _inGrid;

    CT_HandleInResultGroup<>                            _inResultDTM;
    CT_HandleInStdZeroOrMoreGroup                       _inZeroOrMoreRootGroupDTM;
    CT_HandleInStdGroup<>                               _inGroupDTM;
    CT_HandleInSingularItem<CT_Image2D<float> >         _inDTM;

    CT_HandleOutSingularItem<lvox::Grid3Df>             _NORM;

};

#endif // LVOX3_STEPGRIDNORMALISATION_H

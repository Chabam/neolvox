#ifndef LVOX3_STEPGRIDNORMALISATION_H
#define LVOX3_STEPGRIDNORMALISATION_H

#include "ct_step/abstract/ct_abstractstep.h"

#include "tools/lvox3_gridtype.h"

class LVOX3_StepGridNormalisation : public CT_AbstractStep
{
    Q_OBJECT
public:

    LVOX3_StepGridNormalisation();

    QString description() const;

    QString detailledDescription() const;

    CT_VirtualAbstractStep* createNewInstance() const final;

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

    CT_HandleInResultGroupCopy<>                        _inResultDTM;
    CT_HandleInStdZeroOrMoreGroup                       _inZeroOrMoreRootGroupDTM;
    CT_HandleInStdGroup<>                               _inGroupDTM;
    CT_HandleInSingularItem<LVOX3_AbstractGrid3D >      _inGridDTM;

    CT_HandleOutSingularItem<lvox::Grid3Df>             _NORM;
    CT_HandleOutSingularItem<lvox::Grid3Df>             _ELevation;

};

#endif // LVOX3_STEPGRIDNORMALISATION_H

#ifndef CT_STEPENDLOOP_H
#define CT_STEPENDLOOP_H

#include "ctlibstepaddon_global.h"

#include "ct_step/abstract/ct_abstractstep.h"
#include "ct_itemdrawable/ct_loopcounter.h"

class CTLIBSTEPADDON_EXPORT CT_StepEndLoop : public CT_AbstractStep
{
    Q_OBJECT
public:
    CT_StepEndLoop();

    QString description() const override;

    QString detailledDescription() const override;

    QString inputDescription() const override;
    QString outputDescription() const override;
    QString detailsDescription() const override;

    CT_VirtualAbstractStep* createNewInstance() const override;

    CT_VirtualAbstractStep* restartComputeFromStep() const override;

protected:

    void declareInputModels(CT_StepInModelStructureManager& manager) override;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) override;

    void compute() override;

    CT_VirtualAbstractStep*                 m_mustRestartFromStep;

    CT_HandleInResultGroupCopy<>            m_hInResultCopy;
    CT_HandleInStdGroup<>                   m_hInRootGroup;
    CT_HandleInSingularItem<CT_LoopCounter> m_hInLoopCounter;
};

#endif // CT_STEPENDLOOP_H

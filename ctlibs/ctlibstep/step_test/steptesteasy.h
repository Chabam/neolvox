#ifndef STEPTESTEASY_H
#define STEPTESTEASY_H

#include "ct_step/abstract/ct_abstractstep.h"
#include "itemforsteptest.h"

class STepTestEasy : public CT_AbstractStep
{
    Q_OBJECT
    using SuperClass = CT_AbstractStep;

public:
    STepTestEasy();

    CT_VirtualAbstractStep* createNewInstance() const override;

    QList<QString> m_outputGroupsDisplayableName;
    QList<QString> m_outputItemsDisplayableName;

protected:
    void declareInputModels(CT_StepInModelStructureManager& manager) override;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) override;

    void compute() override;

private:
    CT_HandleOutResultGroup                     m_hOutResult;
    CT_HandleOutStdGroup                        m_hOutRootGroup;
    CT_HandleOutSingularItem<ItemForStepTest>   m_hOutItem;
};

#endif // STEPTESTEASY_H

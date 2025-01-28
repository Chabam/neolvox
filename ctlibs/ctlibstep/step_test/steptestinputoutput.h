#ifndef STEPTESTINPUTOUTPUT_H
#define STEPTESTINPUTOUTPUT_H

#include "ct_step/abstract/ct_abstractstep.h"
#include "itemforsteptest.h"

class StepTestInputOutput : public CT_AbstractStep
{
    Q_OBJECT
    using SuperClass = CT_AbstractStep;

public:
    StepTestInputOutput();

    CT_VirtualAbstractStep* createNewInstance() const override;

    QList<QString> m_inputGroupsDisplayableName;
    QList<QString> m_outputGroupsDisplayableName;

    QList<QString> m_inputItemsDisplayableName;
    QList<QString> m_outputItemsDisplayableName;

protected:
    void declareInputModels(CT_StepInModelStructureManager& manager) override;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) override;

    void compute() override;

private:
    CT_HandleInResultGroup<>                    m_hInResult;
    CT_HandleInStdZeroOrMoreGroup               m_hInRootGroup;
    CT_HandleInStdGroup<>                       m_hInGroup;
    CT_HandleInSingularItem<>                   m_hInItem;

    CT_HandleOutResultGroup                     m_hOutResult;
    CT_HandleOutStdGroup                        m_hOutRootGroup;
    CT_HandleOutStdGroup                        m_hOutGroup1;
    CT_HandleOutSingularItem<ItemForStepTest>   m_hOutItem1;
};

#endif // STEPTESTINPUTOUTPUT_H

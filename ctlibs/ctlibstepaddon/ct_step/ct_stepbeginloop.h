#ifndef CT_STEPBEGINLOOP_H
#define CT_STEPBEGINLOOP_H

#include "ctlibstepaddon_global.h"

#include "ct_step/abstract/ct_abstractstepcanbeaddedfirst.h"
#include "ct_itemdrawable/ct_loopcounter.h"

class CTLIBSTEPADDON_EXPORT CT_StepBeginLoop : public CT_AbstractStepCanBeAddedFirst
{
    Q_OBJECT
    using SuperClass = CT_AbstractStepCanBeAddedFirst;

public:
    CT_StepBeginLoop();

    QString description() const override;

    QString detailledDescription() const override;

    bool isSettingsModified() const override;

    CT_VirtualAbstractStep* createNewInstance() const override;

    void addToLogCurrentTurnInformation();

    QSharedPointer<CT_Counter> counter() const;

protected:
    void declareInputModels(CT_StepInModelStructureManager& manager) override;

    void fillPostInputConfigurationDialog(CT_StepConfigurableDialog* configDialog) override;

    void finalizePostSettings() override;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) override;

    void compute() override;

    int                                         m_nTurns;
    QSharedPointer<CT_Counter>                  _counter;

    CT_HandleOutResultGroup                     m_hOutResult;
    CT_HandleOutStdGroup                        m_hOutRootGroup;
    CT_HandleOutSingularItem<CT_LoopCounter>    m_hOutLoopCounter;
};

#endif // CT_STEPBEGINLOOP_H

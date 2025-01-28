#include "steptestinputoutput.h"

#include <QDebug>

StepTestInputOutput::StepTestInputOutput() : SuperClass()
{

}

CT_VirtualAbstractStep* StepTestInputOutput::createNewInstance() const
{
    return NULL;
}

void StepTestInputOutput::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.addResult(m_hInResult);
    manager.setZeroOrMoreRootGroup(m_hInResult, m_hInRootGroup);
    manager.addGroup(m_hInRootGroup, m_hInGroup);
    manager.addItem(m_hInGroup, m_hInItem);
}

void StepTestInputOutput::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResult(m_hOutResult);
    manager.setRootGroup(m_hOutResult, m_hOutRootGroup);
    manager.addGroup(m_hOutRootGroup, m_hOutGroup1);
    manager.addItem(m_hOutGroup1, m_hOutItem1);
}

void StepTestInputOutput::compute()
{
    m_inputGroupsDisplayableName.clear();
    m_outputGroupsDisplayableName.clear();

    m_inputItemsDisplayableName.clear();
    m_outputItemsDisplayableName.clear();

    for(const CT_StandardItemGroup* group : m_hInGroup.iterateInputs(m_hInResult)) {
        m_inputGroupsDisplayableName.append(group->displayableName());
    }

    for(const CT_AbstractSingularItemDrawable* item : m_hInItem.iterateInputs(m_hInResult)) {
        m_inputItemsDisplayableName.append(item->displayableName());
    }

    for(CT_ResultGroup* result : m_hOutResult.iterateOutputs()) {
        for(int i=0; i<8; ++i) {
            CT_StandardItemGroup* rg = new CT_StandardItemGroup();
            //m_outputRootGroupsDisplayableName.append(QString("root group %1").arg(i));
            //rg->setDisplayableName(m_outputRootGroupsDisplayableName.last());
            result->addRootGroup(m_hOutRootGroup, rg);

            CT_StandardItemGroup* g = new CT_StandardItemGroup();
            m_outputGroupsDisplayableName.append(QString("group %1").arg(i));
            g->setDisplayableName(m_outputGroupsDisplayableName.last());
            rg->addGroup(m_hOutGroup1, g);

            ItemForStepTest* item = new ItemForStepTest();
            m_outputItemsDisplayableName.append(QString("out item %1").arg(i));
            item->setDisplayableName(m_outputItemsDisplayableName.last());
            g->addSingularItem(m_hOutItem1, item);
        }
    }
}

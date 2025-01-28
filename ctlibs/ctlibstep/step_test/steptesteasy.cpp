#include "steptesteasy.h"

STepTestEasy::STepTestEasy() : SuperClass()
{
}

CT_VirtualAbstractStep* STepTestEasy::createNewInstance() const
{
    return NULL;
}

void STepTestEasy::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.setNotNeedInputResult();
}

void STepTestEasy::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResult(m_hOutResult);
    manager.setRootGroup(m_hOutResult, m_hOutRootGroup);
    manager.addItem(m_hOutRootGroup, m_hOutItem);
}

void STepTestEasy::compute()
{
    m_outputGroupsDisplayableName.clear();
    m_outputItemsDisplayableName.clear();

    for(CT_ResultGroup* result : m_hOutResult.iterateOutputs()) {

        for(int i=0; i<3; ++i) {
            CT_StandardItemGroup* rg = new CT_StandardItemGroup();
            m_outputGroupsDisplayableName.append(QString("group %1").arg(i));
            rg->setDisplayableName(m_outputGroupsDisplayableName.last());
            result->addRootGroup(m_hOutRootGroup, rg);

            ItemForStepTest* item = new ItemForStepTest();
            m_outputItemsDisplayableName.append(QString("item %1").arg(i));
            item->setDisplayableName(m_outputItemsDisplayableName.last());
            rg->addSingularItem(m_hOutItem, item);
        }
    }
}

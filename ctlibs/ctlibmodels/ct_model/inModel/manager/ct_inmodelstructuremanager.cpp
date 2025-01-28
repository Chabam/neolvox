#include "ct_inmodelstructuremanager.h"

#include "ct_model/inModel/ct_inresultmodelnotneedinputresult.h"

CT_InModelStructureManager::~CT_InModelStructureManager()
{
    clearResults(true);
}

void CT_InModelStructureManager::setNotNeedInputResult()
{
    qDeleteAll(m_results);
    m_results.clear();

    m_results.append(new CT_InResultModelNotNeedInputResult());
}

bool CT_InModelStructureManager::needInputs() const
{
    return !m_results.isEmpty()
            && (dynamic_cast<CT_InResultModelNotNeedInputResult*>(m_results.first()) == nullptr);
}

bool CT_InModelStructureManager::doesAtLeastOneInputModelsUseType(const QString& type) const
{
   bool yes = false;

   visitResults([&yes, &type](const CT_InAbstractResultModel* resultModel) -> bool {
       resultModel->recursiveVisitInChildrens([&yes, &type](const CT_InAbstractModel* model) -> bool {
           const CT_InAbstractItemModel* itemModel = dynamic_cast<const CT_InAbstractItemModel*>(model);
           yes = itemModel->itemType().startsWith(type);
           return !yes;
       });

       return !yes;
   });

   return yes;
}

int CT_InModelStructureManager::nResults() const
{
    return m_results.size();
}

void CT_InModelStructureManager::clearResults(bool deleteFromMemory)
{
    if(deleteFromMemory)
        qDeleteAll(m_results);

    m_results.clear();
}

void CT_InModelStructureManager::checkNotNeedInputResultIsNotPresent() const
{
    Q_ASSERT_X(m_results.isEmpty()
               || (dynamic_cast<CT_InResultModelNotNeedInputResult*>(m_results.first()) == nullptr),
               "CT_InModelStructureManager", "You have already define that you do not want input results so you can not add new elements !");
    if (!m_results.isEmpty() && (dynamic_cast<CT_InResultModelNotNeedInputResult*>(m_results.first()) != nullptr)) {qDebug() << "CT_InModelStructureManager::checkNotNeedInputResultIsNotPresent" << ", " <<  "You have already define that you do not want input results so you can not add new elements !"; return;}

}

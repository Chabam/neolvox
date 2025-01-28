#include "ct_outresultmodelgrouptocopypossibilities.h"

CT_OutResultModelGroupToCopyPossibilities::CT_OutResultModelGroupToCopyPossibilities()
{
}

CT_OutResultModelGroupToCopyPossibilities::~CT_OutResultModelGroupToCopyPossibilities()
{
    qDeleteAll(m_models);
}

void CT_OutResultModelGroupToCopyPossibilities::addResulModel(CT_OutResultModelGroupCopy* rModel)
{
    m_models.append(rModel);
}

int CT_OutResultModelGroupToCopyPossibilities::nResultModels() const
{
    return m_models.size();
}

void CT_OutResultModelGroupToCopyPossibilities::removeRootGroup()
{
    for(CT_OutResultModelGroupCopy* outResultModel : m_models) {
        outResultModel->setRootGroup(nullptr);
    }
}

void CT_OutResultModelGroupToCopyPossibilities::removeGroup(const DEF_CT_AbstractGroupModelIn* inGroupToRemove)
{
    for(CT_OutResultModelGroupCopy* outResultModel : m_models) {
        outResultModel->removeGroup(inGroupToRemove);
    }
}

void CT_OutResultModelGroupToCopyPossibilities::removeItem(const DEF_CT_AbstractItemDrawableModelIn* inItemToRemove)
{
    for(CT_OutResultModelGroupCopy* outResultModel : m_models) {
        outResultModel->removeItem(inItemToRemove);
    }
}

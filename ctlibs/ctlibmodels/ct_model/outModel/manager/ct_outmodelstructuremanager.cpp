#include "ct_outmodelstructuremanager.h"

CT_OutModelStructureManager::~CT_OutModelStructureManager()
{
    clearResults(true);
}

void CT_OutModelStructureManager::setIgnoreInvalidParentHandle(bool ignore)
{
    m_ignoreInvalidParentHandle = ignore;
}

int CT_OutModelStructureManager::nResults() const
{
    return m_results.size();
}

void CT_OutModelStructureManager::clearResults(bool deleteFromMemory)
{
    if(deleteFromMemory)
        qDeleteAll(m_results);

    m_results.clear();
    m_inResultsCopy.clear();
}

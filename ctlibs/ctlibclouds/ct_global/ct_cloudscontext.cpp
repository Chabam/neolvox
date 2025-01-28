#include "ct_cloudscontext.h"

#include "ct_global/ct_repositorymanager.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"

CT_CloudsContext* CT_CloudsContext::UNIQUE_INSTANCE = nullptr;

CT_CloudsContext::CT_CloudsContext()
{
    m_repositoryManager = nullptr;
    m_csManager = nullptr;
}

CT_CloudsContext::~CT_CloudsContext()
{
    delete m_repositoryManager;
    delete m_csManager;
}

CT_RepositoryManager* CT_CloudsContext::repositoryManager() const
{
    return m_repositoryManager;
}

void CT_CloudsContext::init()
{
    m_repositoryManager = new CT_RepositoryManager();
    m_csManager = new CT_CoordinateSystemManager();
}

CT_Repository* CT_CloudsContext::repository() const
{
    return m_repositoryManager->repository();
}

void CT_CloudsContext::informThatWeStartToDeleteMultipleCloud()
{
    repository()->begingDeleteMultiCloud();
}

void CT_CloudsContext::informThatWeHaveFinishedToDeleteMultipleCloud()
{
    repository()->endDeleteMultiCloud();
}

CT_CoordinateSystemManager* CT_CloudsContext::coordinateSystemManager() const
{
    return m_csManager;
}

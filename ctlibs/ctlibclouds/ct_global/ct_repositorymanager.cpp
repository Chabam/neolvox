#include "ct_repositorymanager.h"

#include "ct_repository.h"

CT_RepositoryManager::CT_RepositoryManager()
{
    m_repository = new CT_Repository();
}

CT_RepositoryManager::~CT_RepositoryManager()
{
    delete m_repository;
}

CT_Repository* CT_RepositoryManager::repository()
{
    return m_repository;
}

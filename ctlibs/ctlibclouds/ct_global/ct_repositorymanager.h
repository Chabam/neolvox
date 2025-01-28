#ifndef CT_REPOSITORYMANAGER_H
#define CT_REPOSITORYMANAGER_H

#include "ct_global/ct_repository.h"

class CTLIBCLOUDS_EXPORT CT_RepositoryManager
{
public:
    CT_RepositoryManager();
    ~CT_RepositoryManager();

    /**
     * @brief repository getter
     * @return return the repository
     */
    CT_Repository* repository();

private:
    CT_Repository       *m_repository;
};

#endif // CT_REPOSITORYMANAGER_H

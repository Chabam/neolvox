#ifndef CT_CLOUDSCONTEXT_H
#define CT_CLOUDSCONTEXT_H

#include "ctlibclouds_global.h"

#define PS_CLOUDSCONTEXT CT_CloudsContext::staticInstance()
#define PS_REPOSITORY_MANAGER CT_CloudsContext::staticInstance()->repositoryManager()
#define PS_REPOSITORY CT_CloudsContext::staticInstance()->repositoryManager()->repository()
#define PS_COORDINATES_SYS_MANAGER CT_CloudsContext::staticInstance()->coordinateSystemManager()

class CT_RepositoryManager;
class CT_Repository;
class CT_CoordinateSystemManager;

class CTLIBCLOUDS_EXPORT CT_CloudsContext
{
public:

    static CT_CloudsContext* staticInstance()
    {
        if(UNIQUE_INSTANCE == nullptr) {
            UNIQUE_INSTANCE = new CT_CloudsContext();
            UNIQUE_INSTANCE->init();
        }

        return UNIQUE_INSTANCE;
    }

    static void deleteStaticInstance()
    {
        delete UNIQUE_INSTANCE;
        UNIQUE_INSTANCE = nullptr;
    }

    ~CT_CloudsContext();

    /**
     * @brief Returns the repository manager
     */
    CT_RepositoryManager* repositoryManager() const;

    /**
     * @brief Returns the current repository
     */
    CT_Repository* repository() const;

    /**
     * @brief Returns the coordinate system manager
     */
    CT_CoordinateSystemManager* coordinateSystemManager() const;

    void informThatWeStartToDeleteMultipleCloud();

    void informThatWeHaveFinishedToDeleteMultipleCloud();

private:
    static CT_CloudsContext*        UNIQUE_INSTANCE;
    CT_RepositoryManager*           m_repositoryManager;
    CT_CoordinateSystemManager*     m_csManager;

    CT_CloudsContext();

    void init();
};

#endif // CT_CLOUDSCONTEXT_H

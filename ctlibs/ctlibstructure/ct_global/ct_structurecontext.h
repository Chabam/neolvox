#ifndef CT_STRUCTURECONTEXT_H
#define CT_STRUCTURECONTEXT_H

#include "ctlibstructure_global.h"

#define PS_STRUCTURECONTEXT CT_StructureContext::staticInstance()
#define PS_CATEGORY_MANAGER CT_StructureContext::staticInstance()->categoryManager()
#define PS_DIAM CT_StructureContext::staticInstance()->defaultItemAttributesManager()

class CT_DefaultItemAttributeManager;
class CT_CategoryManager;

class CTLIBSTRUCTURE_EXPORT CT_StructureContext
{
public:

    static CT_StructureContext* staticInstance()
    {
        if(UNIQUE_INSTANCE == nullptr) {
            UNIQUE_INSTANCE = new CT_StructureContext();
            UNIQUE_INSTANCE->init();
        }

        return UNIQUE_INSTANCE;
    }

    static void deleteStaticInstance()
    {
        delete UNIQUE_INSTANCE;
        UNIQUE_INSTANCE = nullptr;
    }

    ~CT_StructureContext();

    /**
     * @brief Returns the category manager for item attributes
     */
    CT_CategoryManager* categoryManager() const;

    /**
     * @brief Returns the default item attributes manager
     */
    CT_DefaultItemAttributeManager* defaultItemAttributesManager() const;

    void informThatWeStartToDeleteMultipleCloud();

    void informThatWeHaveFinishedToDeleteMultipleCloud();

private:
    static CT_StructureContext*         UNIQUE_INSTANCE;
    CT_CategoryManager*                 m_categoryManager;
    CT_DefaultItemAttributeManager*     m_defaultItemAttributeManager;

    CT_StructureContext();

    void init();
};

#endif // CT_STRUCTURECONTEXT_H

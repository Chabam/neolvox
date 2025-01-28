#ifndef CT_CATEGORYMANAGER_H
#define CT_CATEGORYMANAGER_H

#include "ctlibmodels_global.h"
#include <QHash>

class CT_AbstractCategory;

/**
 * @brief The manager of categories. A category must be registered to be used with item attributes.
 */
class CTLIBMODELS_EXPORT CT_CategoryManager
{
public:
    /**
      * @brief Returns the manager to use to register categories
      */
    static CT_CategoryManager* CM();

    CT_CategoryManager(const CT_CategoryManager& other) = delete;
    CT_CategoryManager& operator=(const CT_CategoryManager& other) = delete;
    ~CT_CategoryManager();

    /**
     * @brief Register a new category
     */
    void registerCategory(CT_AbstractCategory *c);

    /**
     * @brief Find a category by a unique name
     * @return nullptr if the category was not found
     */
    const CT_AbstractCategory* findByUniqueName(const QString& uniqueName) const;

    /**
     * @brief Returns all categories registered
     */
    QList<const CT_AbstractCategory*> categories() const;

private:
    friend class CT_StructureContext;

    CT_CategoryManager();

    /**
     * @brief The default category manager
     */
    static CT_CategoryManager* UNIQUE_INSTANCE;

    QHash<QString, const CT_AbstractCategory*> m_categories;
};

#endif // CT_CATEGORYMANAGER_H

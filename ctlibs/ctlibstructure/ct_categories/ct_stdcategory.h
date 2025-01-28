#ifndef CT_STDCATEGORY_H
#define CT_STDCATEGORY_H

#include "ct_categories/abstract/ct_abstractcategory.h"
#include "ctlibstructure_global.h"

/**
 * @brief A standard category. See \def CT_AbstractCategoryClass class for more information
 */
class CTLIBSTRUCTURE_EXPORT CT_StdCategory : public CT_AbstractCategory
{
public:

    /**
     * @brief Construct a category
     * @param uniqueName : a category must have a unique name to find it in a list
     * @param valueType : the type of the value (must be at end of the hierarchy : only one bit to 1)
     * @param categoryEquivalentUniqueName : a unique name of a category that can be equivalent to this category
     * @param displayableName : a displayable name (by default the unique name if empty)
     * @param description : a description
     */
    CT_StdCategory(const QString &uniqueName,
                   const QString &categoryEquivalentUniqueName = "",
                   const QString &displayableName = "",
                   const QString &description = "");

    /**
     * @brief Construct a category
     * @param uniqueName : a category must have a unique name to find it in a list
     * @param valueType : the type of the value (must be at end of the hierarchy : only one bit is set)
     * @param categoryEquivalentUniqueNameCollection : a collection of unique name of categories that can be equivalent to this category
     * @param displayableName : a displayable name (by default the unique name if empty)
     * @param description : a description
     */
    CT_StdCategory(const QString &uniqueName,
                   const QList<QString> &categoryEquivalentUniqueNameCollection,
                   const QString &displayableName = "",
                   const QString &description = "");

    bool isEquivalentTo(const CT_AbstractCategory* c) const final;
    bool isEquivalentTo(const QString &categoryUniqueName) const final;
};

#endif // CT_STDCATEGORY_H

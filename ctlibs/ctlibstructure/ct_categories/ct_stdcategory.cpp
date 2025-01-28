#include "ct_stdcategory.h"

#include "ct_global/ct_structurecontext.h"
#include "ct_categories/tools/ct_categorymanager.h"

CT_StdCategory::CT_StdCategory(const QString &uniqueName,
                               const QString &categoryEquivalentUniqueName,
                               const QString &displayableName,
                               const QString &description) : CT_AbstractCategory(uniqueName,
                                                                                 categoryEquivalentUniqueName,
                                                                                 displayableName,
                                                                                 description)
{

}

CT_StdCategory::CT_StdCategory(const QString &uniqueName,
                               const QList<QString> &categoryEquivalentUniqueNameCollection,
                               const QString &displayableName,
                               const QString &description) : CT_AbstractCategory(uniqueName,
                                                                                 categoryEquivalentUniqueNameCollection,
                                                                                 displayableName,
                                                                                 description)
{

}

bool CT_StdCategory::isEquivalentTo(const CT_AbstractCategory* c) const
{
    if(m_eCategories.contains(c->uniqueName()))
        return true;

    foreach (const QString &v, m_eCategories) {
        if(PS_CATEGORY_MANAGER->findByUniqueName(v)->isEquivalentTo(c))
            return true;
    }

    return false;
}

bool CT_StdCategory::isEquivalentTo(const QString &categoryUniqueName) const
{
    if(m_eCategories.contains(categoryUniqueName))
        return true;

    foreach (const QString &v, m_eCategories) {
        const ICategoryForModel* c = PS_CATEGORY_MANAGER->findByUniqueName(v);
        if(c->isEquivalentTo(categoryUniqueName))
            return true;
    }

    return false;
}

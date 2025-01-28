#include "ct_categorymanager.h"

#include "ct_categories/abstract/ct_abstractcategory.h"

CT_CategoryManager* CT_CategoryManager::UNIQUE_INSTANCE = nullptr;

CT_CategoryManager* CT_CategoryManager::CM()
{
    return UNIQUE_INSTANCE;
}

CT_CategoryManager::CT_CategoryManager()
{
    assert(UNIQUE_INSTANCE == nullptr);
    if (UNIQUE_INSTANCE != nullptr) {qDebug() << "CT_CategoryManager::CT_CategoryManager" << ", " <<  "UNIQUE_INSTANCE != nullptr";}

    UNIQUE_INSTANCE = this;
}

CT_CategoryManager::~CT_CategoryManager()
{
    qDeleteAll(m_categories.begin(), m_categories.end());
}

void CT_CategoryManager::registerCategory(CT_AbstractCategory *c)
{
    m_categories.insert(c->uniqueName(), c);
}

const CT_AbstractCategory* CT_CategoryManager::findByUniqueName(const QString& uniqueName) const
{
    return m_categories.value(uniqueName, nullptr);
}

QList<const CT_AbstractCategory *> CT_CategoryManager::categories() const
{
    return m_categories.values();
}

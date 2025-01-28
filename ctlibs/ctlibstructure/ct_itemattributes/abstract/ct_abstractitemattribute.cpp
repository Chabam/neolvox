#include "ct_abstractitemattribute.h"

#include "ct_model/outModel/abstract/ct_outabstractitemattributemodel.h"

#include "ct_global/ct_structurecontext.h"
#include "ct_categories/tools/ct_categorymanager.h"

CT_AbstractItemAttribute::CT_AbstractItemAttribute() : SuperClass()
{
    m_iaTool.m_pointer = this;
    m_category = nullptr;
}

CT_AbstractItemAttribute::CT_AbstractItemAttribute(const CT_AbstractItemAttribute& other) : SuperClass(other),
    m_category(other.m_category)
{
    m_iaTool.m_pointer = this;
}

bool CT_AbstractItemAttribute::visitChildrens(const CT_AbstractItem::ChildrensVisitor&) const
{
    /* no childrens */
    return true;
}

int CT_AbstractItemAttribute::nChildrens() const
{
    /* no childrens */
    return 0;
}

CT_AbstractItemAttribute::CT_AbstractItemAttribute(const CT_AbstractCategory* category) : SuperClass()
{
    m_iaTool.m_pointer = this;

    Q_ASSERT_X(category != nullptr, "CT_AbstractItemAttribute constructor", "When you create a ItemAttribute the category must not be null !");
    if (category == nullptr) {qDebug() << "CT_AbstractItemAttribute::CT_AbstractItemAttribute" << ", " << "When you create a ItemAttribute the category must not be null !";}

    m_category = const_cast<CT_AbstractCategory*>(category);
}

CT_AbstractItemAttribute::CT_AbstractItemAttribute(const QString& categoryName) : CT_AbstractItemAttribute(PS_CATEGORY_MANAGER->findByUniqueName(categoryName))
{
}

bool CT_AbstractItemAttribute::isValid() const
{
    return valueType() != CT_AbstractCategory::UNKNOWN;
}

QString CT_AbstractItemAttribute::displayableName() const
{
    if(SuperClass::model() != nullptr)
        return SuperClass::model()->displayableName();
    else if(m_category != nullptr)
        return m_category->displayableName();

    return QString();
}

void CT_AbstractItemAttribute::setCategory(const CT_AbstractCategory* category)
{
    Q_ASSERT(m_category == nullptr);
    if (category == nullptr) {qDebug() << "CT_AbstractItemAttribute::setCategory" << ", " << "category == nullptr";}

    m_category = const_cast<CT_AbstractCategory*>(category);
}

CT_AbstractCategory* CT_AbstractItemAttribute::category() const
{
    return m_category;
}

void CT_AbstractItemAttribute::setItemAttributeModel(const DEF_CT_OutAbstractIAModel* model)
{
    setModel(model);
}

IPrototypeToolForModel*CT_AbstractItemAttribute::prototypeToolForModel() const
{
    return SuperClass::prototypeToolForModel();
}

IItemAttributeToolForModel* CT_AbstractItemAttribute::itemAttributeToolForModel() const
{
    return &const_cast<ItemAttributeToolForModel&>(m_iaTool);
}

CT_AbstractItemAttribute::IChildrensIteratorQtStylePtr CT_AbstractItemAttribute::createQtStyleIteratorForChildrensThatUseOutModel(const CT_OutAbstractModel*) const
{
    return nullptr; // no childrens
}

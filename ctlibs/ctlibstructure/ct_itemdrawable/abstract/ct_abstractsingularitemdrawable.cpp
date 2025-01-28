#include "ct_abstractsingularitemdrawable.h"

#include "ct_itemdrawable/ct_standarditemgroup.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"

CT_DEFAULT_IA_INIT(CT_AbstractSingularItemDrawable)

CT_AbstractSingularItemDrawable::CT_AbstractSingularItemDrawable() : SuperClass()
{
    m_defaultColor = Qt::white;
}

CT_AbstractSingularItemDrawable::CT_AbstractSingularItemDrawable(const CT_AbstractSingularItemDrawable& other) : SuperClass(other),
    m_defaultColor(other.m_defaultColor)
{
    // TODO : MK 20.11.18 copy item attributes
}

double CT_AbstractSingularItemDrawable::centerX() const
{
    return 0;
}

double CT_AbstractSingularItemDrawable::centerY() const
{
    return 0;
}

double CT_AbstractSingularItemDrawable::centerZ() const
{
    return 0;
}

bool CT_AbstractSingularItemDrawable::visitChildrens(const CT_AbstractItem::ChildrensVisitor& visitor) const
{
    const auto visitorAdapter = [&visitor](const CT_AbstractItemAttribute* attribute) -> bool {
        return visitor(attribute);
    };

    if(!PS_DIAM->visitItemAttributesForType(type(), visitorAdapter))
        return false;

    return m_itemAttributes.visitItemAttributes(visitorAdapter);
}

int CT_AbstractSingularItemDrawable::nChildrens() const
{
    return nItemAttributes();
}

void CT_AbstractSingularItemDrawable::addItemAttributeWithOutModel(const CT_OutAbstractItemAttributeModel* outModel, CT_AbstractItemAttribute* itemAttribute)
{
    Q_ASSERT(itemAttribute != nullptr);
    if (itemAttribute == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::addItemAttributeWithOutModel" << ", " << "itemAttribute == nullptr";}

    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::addItemAttributeWithOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(outModel->parentModel() != nullptr);
    if (outModel->parentModel() == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::addItemAttributeWithOutModel" << ", " << "outModel->parentModel() == nullptr";}

    Q_ASSERT(static_cast<CT_OutAbstractModel*>(outModel->parentModel())->recursiveOriginalModel() == model()->recursiveOriginalModel());
    if (static_cast<CT_OutAbstractModel*>(outModel->parentModel())->recursiveOriginalModel() != model()->recursiveOriginalModel()) {qDebug() << "CT_AbstractSingularItemDrawable::addItemAttributeWithOutModel" << ", " << "static_cast<CT_OutAbstractModel*>(outModel->parentModel())->recursiveOriginalModel() != model()->recursiveOriginalModel()";}

    Q_ASSERT(outModel->itemAttribute()->itemAttributeToolForModel()->category() == itemAttribute->category());
    if (outModel->itemAttribute()->itemAttributeToolForModel()->category() != itemAttribute->category()) {qDebug() << "CT_AbstractSingularItemDrawable::addItemAttributeWithOutModel" << ", " << "outModel->itemAttribute()->itemAttributeToolForModel()->category() != itemAttribute->category()";}

    itemAttribute->setModel(outModel);
    itemAttribute->setResult(static_cast<CT_AbstractResult*>(outModel->result()));

    m_itemAttributes.addItemAttribute(itemAttribute);
}

CT_AbstractItemAttribute* CT_AbstractSingularItemDrawable::itemAttributeWithOutModel(const CT_OutAbstractItemAttributeModel* outModel) const
{
    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::itemAttributeWithOutModel" << ", " << "outModel == nullptr";}

    if(outModel->isADefaultItemAttributeModel())
        return PS_DIAM->itemAttributeFromOutModel(outModel, type());

    return m_itemAttributes.itemAttributeFromOutModel(outModel);
}

bool CT_AbstractSingularItemDrawable::visitItemAttributes(const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const
{
    if(!PS_DIAM->visitItemAttributesForType(type(), visitor))
        return false;

    return m_itemAttributes.visitItemAttributes(visitor);
}

bool CT_AbstractSingularItemDrawable::visitDefaultItemAttributes(const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const
{
    return PS_DIAM->visitItemAttributesForType(type(), visitor);
}

bool CT_AbstractSingularItemDrawable::visitItemAttributesAdded(const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const
{
    return m_itemAttributes.visitItemAttributes(visitor);
}

bool CT_AbstractSingularItemDrawable::visitItemAttributesInSelectedPossibilitiesOfInModel(const CT_InAbstractItemAttributeModel* inModel, const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const
{
    Q_ASSERT(inModel != nullptr);
    if (inModel == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::visitItemAttributesInSelectedPossibilitiesOfInModel" << ", " << "inModel == nullptr";}

    if(!PS_DIAM->visitItemAttributesForTypeInSelectedPossibilitiesOfInModel(type(), inModel, visitor))
        return false;

    return m_itemAttributes.visitItemAttributesOfInModel(inModel, visitor);
}

int CT_AbstractSingularItemDrawable::nItemAttributes() const
{
    return nDefaultItemAttributes() + nItemAttributesAdded();
}

int CT_AbstractSingularItemDrawable::nDefaultItemAttributes() const
{
    return PS_DIAM->nItemAttributesForType(type());
}

int CT_AbstractSingularItemDrawable::nItemAttributesAdded() const
{
    return m_itemAttributes.nItemAttributes();
}

QList<CT_AbstractItemAttribute*> CT_AbstractSingularItemDrawable::itemAttributes(const CT_InAbstractItemAttributeModel *inModel) const
{
    Q_ASSERT(inModel != nullptr);
    if (inModel == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::itemAttributes" << ", " << "inModel == nullptr";}

    QList<CT_AbstractItemAttribute*> l = m_itemAttributes.itemAttributesFromInModel(inModel);
    l.append(PS_DIAM->itemAttributesFromInModel(inModel, type()));

    return l;
}

QList<CT_AbstractItemAttribute *> CT_AbstractSingularItemDrawable::itemAttributes(const QList<CT_OutAbstractItemAttributeModel *>& outModelList) const
{
    QList<CT_AbstractItemAttribute *> l;

    QString myType = type();

    for(const CT_OutAbstractItemAttributeModel* outModel : outModelList) {

        CT_AbstractItemAttribute* att = outModel->isADefaultItemAttributeModel() ? PS_DIAM->itemAttributeFromOutModel(outModel, myType) : m_itemAttributes.itemAttributeFromOutModel(outModel);

        if(att != nullptr)
            l.append(att);
    }

    return l;
}

QList<CT_AbstractItemAttribute *> CT_AbstractSingularItemDrawable::itemAttributes() const
{
    QList<CT_AbstractItemAttribute *> l;

    l.append(PS_DIAM->itemAttributes(type()));
    l.append(m_itemAttributes.itemAttributes());

    return l;
}

CT_AbstractItemAttribute* CT_AbstractSingularItemDrawable::firstItemAttribute(const CT_InAbstractItemAttributeModel* inModel) const
{
    Q_ASSERT(inModel != nullptr);
    if (inModel == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::firstItemAttribute" << ", " << "inModel == nullptr";}

    if(!inModel->isAtLeastOnePossibilitySelected())
        return nullptr;

    CT_InStdModelPossibility* p = nullptr;

    const auto visitor = [&p](const CT_InStdModelPossibility* possibility) -> bool {
        p = const_cast<CT_InStdModelPossibility*>(possibility);
        return false;
    };

    inModel->possibilitiesGroup()->visitSelectedPossibilities(visitor);

    return itemAttributeWithOutModel(static_cast<CT_OutAbstractItemAttributeModel*>(p->outModel()));
}

QList<CT_AbstractItemAttribute *> CT_AbstractSingularItemDrawable::defaultItemAttributes() const
{
    return PS_DIAM->itemAttributes(type());
}

QList<CT_AbstractItemAttribute *> CT_AbstractSingularItemDrawable::itemAttributesAdded() const
{
    return m_itemAttributes.itemAttributes();
}

void CT_AbstractSingularItemDrawable::setDefaultColor(const QColor &color)
{
    m_defaultColor = color;
}

QColor CT_AbstractSingularItemDrawable::defaultColor() const
{
    return m_defaultColor;
}

void CT_AbstractSingularItemDrawable::willBeRemovedFromResult(const CT_AbstractResult* res)
{
    m_itemAttributes.removeItemAttributeFromResult(res);
}

void CT_AbstractSingularItemDrawable::setParentGroup(const CT_StandardItemGroup* pGroup)
{
    setParentItem(pGroup);
}

CT_StandardItemGroup* CT_AbstractSingularItemDrawable::parentGroup() const
{
    return static_cast<CT_StandardItemGroup*>(parentItem());
}

CT_AbstractItem::IChildrensIteratorQtStylePtr CT_AbstractSingularItemDrawable::createQtStyleIteratorForChildrensThatUseOutModel(const CT_OutAbstractModel* outModel) const
{
    const CT_OutAbstractItemAttributeModel* iaModel = dynamic_cast<const CT_OutAbstractItemAttributeModel*>(outModel);

    if(iaModel != nullptr) {
        const CT_AbstractItemAttribute* attribute = iaModel->isADefaultItemAttributeModel() ? PS_DIAM->itemAttributeFromOutModel(iaModel, type()) : m_itemAttributes.itemAttributeFromOutModel(iaModel);

        if(attribute != nullptr)
            return IChildrensIteratorQtStylePtr(new ItemAttributeIterator(attribute));
    }

    return IChildrensIteratorQtStylePtr(nullptr);
}

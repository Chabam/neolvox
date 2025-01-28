#include "ct_itemattributecontainer.h"

#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"
#include "ct_result/abstract/ct_abstractresult.h"

// TODO : MK 21.11.18 Check if unique index comparaison can be removed and compare only address of out model.

CT_ItemAttributeContainer::CT_ItemAttributeContainer()
{
}

CT_ItemAttributeContainer::~CT_ItemAttributeContainer()
{
    clear();
}

bool CT_ItemAttributeContainer::addItemAttribute(CT_AbstractItemAttribute *att)
{
    QList<CT_AbstractItemAttribute *> *l = m_attributes.value(att->result(), nullptr);

    if(l == nullptr)
    {
        l = new QList<CT_AbstractItemAttribute *>();
        m_attributes.insert(att->result(), l);
    }

    l->append(att);
    return true;
}

void CT_ItemAttributeContainer::removeItemAttribute(CT_AbstractItemAttribute *att)
{
    CT_AbstractResult *res = att->result();
    QList<CT_AbstractItemAttribute *> *l = m_attributes.value(res, nullptr);

    if(l != nullptr)
    {
        if(l->removeOne(att))
            delete att;

        if(l->isEmpty())
        {
            m_attributes.remove(res);
            delete l;
        }
    }
}

void CT_ItemAttributeContainer::removeItemAttributeFromResult(const CT_AbstractResult *result)
{
    QList<CT_AbstractItemAttribute *>* l = m_attributes.take(const_cast<CT_AbstractResult*>(result));

    if(l != nullptr)
        qDeleteAll(l->begin(), l->end());

    delete l;
}

bool CT_ItemAttributeContainer::visitItemAttributes(const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const
{
    QHashIterator<CT_AbstractResult*, QList<CT_AbstractItemAttribute*>*> it(m_attributes);

    while(it.hasNext()) {
        it.next();

        const QList<CT_AbstractItemAttribute*>* l = it.value();

        for(const CT_AbstractItemAttribute* att : *l) {
            if(!visitor(att))
                return false;
        }
    }

    return true;
}

bool CT_ItemAttributeContainer::visitItemAttributesOfResult(const CT_AbstractResult* result, const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const
{
    const QList<CT_AbstractItemAttribute*>* l = m_attributes.value(const_cast<CT_AbstractResult*>(result), nullptr);

    if(l != nullptr) {
        for(const CT_AbstractItemAttribute* att : *l) {
            if(!visitor(att))
                return false;
        }
    }

    return true;
}

bool CT_ItemAttributeContainer::visitItemAttributesOfOutModel(const CT_OutAbstractItemAttributeModel* outModel, const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const
{
    const QList<CT_AbstractItemAttribute*>* l = m_attributes.value(static_cast<CT_AbstractResult*>(outModel->recursiveOriginalModel()->result()), nullptr);

    if(l != nullptr) {
        const CT_OutAbstractModel::UniqueIndexType uIndex = outModel->uniqueIndex();

        for(const CT_AbstractItemAttribute* att : *l) {

            // TODO : MK 23.11.18 check if unique index is necessary ?
            if(att->model()->uniqueIndex() == uIndex) {
                if(!visitor(att))
                    return false;
            }
        }
    }

    return true;
}

bool CT_ItemAttributeContainer::visitItemAttributesOfInModel(const CT_InAbstractItemAttributeModel* inModel, const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const
{
    const auto visitorAdapter = [this, &visitor](const CT_InStdModelPossibility* possibility) -> bool {

        const QList<CT_AbstractItemAttribute*>* collection = m_attributes.value(static_cast<CT_AbstractResult*>(possibility->outModel()->recursiveOriginalModel()->result()), nullptr);

        if(collection != nullptr) {
            const CT_OutAbstractModel::UniqueIndexType uIndex = possibility->outModel()->uniqueIndex();

            for(CT_AbstractItemAttribute* att : *collection)
            {
                // TODO : MK 23.11.18 check if unique index is necessary ?
                if(att->model()->uniqueIndex() == uIndex) {
                    if(!visitor(att))
                        return false;
                }
            }
        }

        return true;
    };

    return inModel->possibilitiesGroup()->visitSelectedPossibilities(visitorAdapter);
}

int CT_ItemAttributeContainer::nItemAttributes() const
{
    int n = 0;

    for(const auto v : m_attributes) {
        n += v->size();
    }

    return n;
}

QList<CT_AbstractItemAttribute *> CT_ItemAttributeContainer::itemAttributes() const
{
    QList<CT_AbstractItemAttribute*> ret;

    QList< QList<CT_AbstractItemAttribute*>* > l = m_attributes.values();
    QListIterator< QList<CT_AbstractItemAttribute*>* > it(l);

    while(it.hasNext())
        ret.append(*it.next());

    return ret;
}

QList<CT_AbstractItemAttribute *> CT_ItemAttributeContainer::itemAttributesFromResult(const CT_AbstractResult* result) const
{
    QList<CT_AbstractItemAttribute *>* l = m_attributes.value(const_cast<CT_AbstractResult*>(result), nullptr);

    if(l == nullptr)
        return QList<CT_AbstractItemAttribute *>();

    return *l;
}

CT_AbstractItemAttribute* CT_ItemAttributeContainer::itemAttributeFromOutModel(const CT_OutAbstractItemAttributeModel* outModel) const
{
    const QList<CT_AbstractItemAttribute*>* collection = m_attributes.value(static_cast<CT_AbstractResult*>(outModel->recursiveOriginalModel()->result()), nullptr);

    if(collection != nullptr) {
        const CT_OutAbstractModel::UniqueIndexType uIndex = outModel->uniqueIndex();

        for(CT_AbstractItemAttribute* att : *collection)
        {
            // TODO : MK 23.11.18 check if unique index is necessary ?
            if(att->model()->uniqueIndex() == uIndex)
                return att;
        }
    }

    return nullptr;
}

QList<CT_AbstractItemAttribute*> CT_ItemAttributeContainer::itemAttributesFromInModel(const CT_InAbstractItemAttributeModel* inModel) const
{
    QList<CT_AbstractItemAttribute*> l;

    const auto visitor = [this, &l](const CT_InStdModelPossibility* possibility) -> bool {

        const QList<CT_AbstractItemAttribute*>* collection = m_attributes.value(static_cast<CT_AbstractResult*>(possibility->outModel()->recursiveOriginalModel()->result()), nullptr);

        if(collection != nullptr) {
            const CT_OutAbstractModel::UniqueIndexType uIndex = possibility->outModel()->uniqueIndex();

            for(CT_AbstractItemAttribute* att : *collection)
            {
                // TODO : MK 23.11.18 check if unique index is necessary ?
                if(att->model()->uniqueIndex() == uIndex)
                    l.append(att);
            }
        }

        return true;
    };

    inModel->possibilitiesGroup()->visitSelectedPossibilities(visitor);

    return l;
}

CT_AbstractItemAttribute* CT_ItemAttributeContainer::firstItemAttributeFromInModel(const CT_InAbstractItemAttributeModel* inModel) const
{
    CT_AbstractItemAttribute* found = nullptr;

    const auto visitor = [this, &found](const CT_InStdModelPossibility* possibility) -> bool {

        const QList<CT_AbstractItemAttribute*>* collection = m_attributes.value(static_cast<CT_AbstractResult*>(possibility->outModel()->recursiveOriginalModel()->result()), nullptr);

        if(collection != nullptr) {
            const CT_OutAbstractModel::UniqueIndexType uIndex = possibility->outModel()->uniqueIndex();

            for(CT_AbstractItemAttribute* att : *collection)
            {
                // TODO : MK 23.11.18 check if unique index is necessary ?
                if(att->model()->uniqueIndex() == uIndex) {
                    found = att;
                    return false;
                }
            }
        }

        return true;
    };

    inModel->possibilitiesGroup()->visitSelectedPossibilities(visitor);

    return found;
}

void CT_ItemAttributeContainer::clear()
{
    QHashIterator<CT_AbstractResult*, QList<CT_AbstractItemAttribute *>* > it(m_attributes);

    while(it.hasNext())
    {
        it.next();
        QList<CT_AbstractItemAttribute *> *l = it.value();
        qDeleteAll(l->begin(), l->end());
        delete l;
    }

    m_attributes.clear();
}

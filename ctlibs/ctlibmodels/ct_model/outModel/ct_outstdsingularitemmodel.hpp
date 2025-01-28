#include "ct_outstdsingularitemmodel.h"

template<class SingularItem>
CT_OutStdSingularItemModel<SingularItem>::CT_OutStdSingularItemModel(const QString& displayableName,
                                                                     const QString& shortDescription,
                                                                     const QString& detailledDescription,
                                                                     SingularItem* prototype) : SuperClass(displayableName)
{
    setPrototype((prototype == nullptr) ? new SingularItem() : prototype);
    setShortDescription(shortDescription);
    setDetailledDescription(detailledDescription);
}

template<class SingularItem>
CT_OutStdSingularItemModel<SingularItem>::CT_OutStdSingularItemModel(const CT_OutStdSingularItemModel& other) : SuperClass(other)
{
    // copy attributes
    const auto visitor = [this](const CT_OutAbstractItemAttributeModel* attributeModel) -> bool {
        this->addItemAttribute(static_cast<CT_OutAbstractItemAttributeModel*>(attributeModel->copy()));
        return true;
    };

    other.visitAttributes(visitor);
}

template<class SingularItem>
CT_OutAbstractModel* CT_OutStdSingularItemModel<SingularItem>::copy() const
{
    return new CT_OutStdSingularItemModel(*this);
}

template<class SingularItem>
bool CT_OutStdSingularItemModel<SingularItem>::finalize()
{
    const IItemDrawableForModel* itemD = itemDrawable();

    if(itemD != nullptr)
    {
        QList<IItemAttributeForModel*> l;

        const auto visitorIA = [&l](const IItemAttributeForModel* attribute) -> bool {
            l.append(const_cast<IItemAttributeForModel*>(attribute));
            return true;
        };

        // get default item attributes of the item drawable prototype
        itemDrawable()->itemToolForModel()->visitItemHisDefaultAttributes(visitorIA);

        // remove from the collection all item attributes that already exist (in the case this model has been copied)
        const auto visitor = [&l](const CT_OutAbstractItemAttributeModel* attributeModel) -> bool {

            QMutableListIterator<IItemAttributeForModel*> it(l);

            while(it.hasNext())
            {
                const IItemAttributeForModel* att = it.next();

                if(attributeModel->recursiveOriginalModel() == att->prototypeToolForModel()->model()->recursiveOriginalModel())
                    it.remove();
            }

            return true;
        };

        visitAttributes(visitor);

        // add item attributes that has not been removed from the collection (in case
        // this model has been copied the collection is empty normally)
        for(const IItemAttributeForModel* att : l)
        {
            // get the model of this attribute
            CT_OutAbstractItemAttributeModel* attModel = static_cast<CT_OutAbstractItemAttributeModel*>(att->prototypeToolForModel()->model());

            // copy the model
            CT_OutAbstractItemAttributeModel* cpy = static_cast<CT_OutAbstractItemAttributeModel*>(attModel->copy());

            // add it to this model
            addItemAttribute(cpy);
        }
    }

    return DEF_CT_AbstractItemDrawableModelOut::finalize();
}

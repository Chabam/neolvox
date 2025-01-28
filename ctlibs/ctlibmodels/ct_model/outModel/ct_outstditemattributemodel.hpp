#include "ct_outstditemattributemodel.h"

#include <QDebug>

template<class ItemAttribute>
CT_OutStdItemAttributeModel<ItemAttribute>::CT_OutStdItemAttributeModel(const ICategoryForModel* category,
                                                                        const QString& displayableName,
                                                                        const QString& shortDescription,
                                                                        const QString& detailledDescription,
                                                                        ItemAttribute* prototype) : SuperClass(displayableName)
{
    if(prototype == nullptr) {
        prototype = new ItemAttribute();
        prototype->setCategory(static_cast<const typename ItemAttribute::CategoryType*>(category));
    }

    Q_ASSERT(prototype->category() == category);
    if (prototype->category() != category) {qDebug() << "CT_OutStdItemAttributeModel<ItemAttribute>::CT_OutStdItemAttributeModel" << ", " <<  "prototype->category() != category"; return;}

    setPrototype(prototype);
    setShortDescription(shortDescription);
    setDetailledDescription(detailledDescription);
}

template<class ItemAttribute>
CT_OutStdItemAttributeModel<ItemAttribute>::CT_OutStdItemAttributeModel(const QString& categoryUniqueName,
                                                                        const QString& displayableName,
                                                                        const QString& shortDescription,
                                                                        const QString& detailledDescription,
                                                                        ItemAttribute* prototype) : CT_OutStdItemAttributeModel(CT_CategoryManager::CM()->findByUniqueName(categoryUniqueName),
                                                                                                                                displayableName,
                                                                                                                                shortDescription,
                                                                                                                                detailledDescription,
                                                                                                                                prototype)
{
}

template<class ItemAttribute>
CT_OutAbstractModel* CT_OutStdItemAttributeModel<ItemAttribute>::copy() const
{
    return new CT_OutStdItemAttributeModel(*this);
}

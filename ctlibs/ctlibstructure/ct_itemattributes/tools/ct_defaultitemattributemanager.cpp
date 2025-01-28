#include "ct_defaultitemattributemanager.h"

#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_itemdrawable/abstract/ct_abstractitemdrawable.h"

CT_DefaultItemAttributeManager::CT_DefaultItemAttributeManager()
{
}

CT_DefaultItemAttributeManager::~CT_DefaultItemAttributeManager()
{
    qDeleteAll(m_collection.begin(), m_collection.end());
}

bool CT_DefaultItemAttributeManager::visitItemAttributesForType(const QString& type, const CT_DefaultItemAttributeManager::ItemAttributesVisitor& visitor) const
{
    // TODO : test performance en utilisant le type et sans utiliser le type (juste parcourir toute la map)
    const QStringList types = type.split("/");
    bool ok = false;

    for (const QString& type : types) {

        // start only from the CT_AbstractSingularItemDrawable class
        if(ok || (type == "CT_AbstractSingularItemDrawable"))
        {
            ok = true;

            DefaultItemAttributesCollection* container = m_collection.value(type, nullptr);

            if(container != nullptr)
            {
                for(const CT_OutAbstractItemAttributeModel* model : *container) {
                    if(!visitor(static_cast<CT_AbstractItemAttribute*>(model->itemAttribute())))
                        return false;
                }
            }
        }
    }

    return true;
}

bool CT_DefaultItemAttributeManager::visitItemAttributeModelsForType(const QString& type, const ItemAttributeModelsVisitor& visitor) const
{
    // TODO : test performance en utilisant le type et sans utiliser le type (juste parcourir toute la map)
    const QStringList types = type.split("/");
    bool ok = false;

    for (const QString& type : types) {

        // start only from the CT_AbstractSingularItemDrawable class
        if(ok || (type == "CT_AbstractSingularItemDrawable"))
        {
            ok = true;

            DefaultItemAttributesCollection* container = m_collection.value(type, nullptr);

            if(container != nullptr)
            {
                for(const CT_OutAbstractItemAttributeModel* model : *container) {
                    if(!visitor(model))
                        return false;
                }
            }
        }
    }

    return true;
}

bool CT_DefaultItemAttributeManager::visitItemAttributesForTypeInSelectedPossibilitiesOfInModel(const QString& itemType,
                                                                                                const CT_InAbstractItemAttributeModel* inModel,
                                                                                                const CT_DefaultItemAttributeManager::ItemAttributesVisitor& visitor) const
{
    QSet<CT_OutAbstractItemAttributeModel*> originalOutModels;

    const auto visitorP = [&originalOutModels](const CT_InStdModelPossibility* possibility) -> bool {
        CT_OutAbstractItemAttributeModel* m = static_cast<CT_OutAbstractItemAttributeModel*>(possibility->outModel()->recursiveOriginalModel());

        if(m->isADefaultItemAttributeModel())
            originalOutModels.insert(m);

        return true;
    };

    inModel->possibilitiesGroup()->visitSelectedPossibilities(visitorP);

    if(!originalOutModels.isEmpty()) {

        const auto visitorAdapter = [&originalOutModels, &visitor](const CT_OutAbstractItemAttributeModel* model) -> bool {
            if(originalOutModels.contains(const_cast<CT_OutAbstractItemAttributeModel*>(model)))
                return visitor(static_cast<CT_AbstractItemAttribute*>(model->itemAttribute()));

            return true;
        };

        return visitItemAttributeModelsForType(itemType, visitorAdapter);
    }

    return true;
}

int CT_DefaultItemAttributeManager::nItemAttributesForType(const QString& itemType) const
{
    int n = 0;

    // TODO : test performance en utilisant le type et sans utiliser le type (juste parcourir toute la map)
    const QStringList types = itemType.split("/");
    bool ok = false;

    for (const QString& type : types) {

        // start only from the CT_AbstractSingularItemDrawable class
        if(ok || (type == "CT_AbstractSingularItemDrawable"))
        {
            ok = true;

            DefaultItemAttributesCollection* container = m_collection.value(type, nullptr);

            if(container != nullptr)
                n += container->size();
        }
    }

    return n;
}

QList<CT_AbstractItemAttribute *> CT_DefaultItemAttributeManager::itemAttributes(const QString &itemType) const
{
    QList<CT_AbstractItemAttribute *> l;

    const auto visitor = [&l](const CT_AbstractItemAttribute* att) -> bool {
        l.append(const_cast<CT_AbstractItemAttribute*>(att));
        return true;
    };

    visitItemAttributesForType(itemType, visitor);

    return l;
}

CT_AbstractItemAttribute* CT_DefaultItemAttributeManager::itemAttributeFromOutModel(const CT_OutAbstractItemAttributeModel* outModel, const QString& itemType) const
{
    CT_OutAbstractItemAttributeModel* orModel = static_cast<CT_OutAbstractItemAttributeModel*>(outModel->recursiveOriginalModel());

    if(!orModel->isADefaultItemAttributeModel())
        return nullptr;

    CT_AbstractItemAttribute* att = nullptr;

    const auto visitor = [&att, &orModel](const CT_OutAbstractItemAttributeModel* model) -> bool {
        if(model == orModel) {
            att = static_cast<CT_AbstractItemAttribute*>(model->itemAttribute());
            return false;
        }

        return true;
    };

    visitItemAttributeModelsForType(itemType, visitor);

    return att;
}

QList<CT_AbstractItemAttribute*> CT_DefaultItemAttributeManager::itemAttributesFromInModel(const CT_InAbstractItemAttributeModel* inModel, const QString& itemType) const
{
    QSet<CT_AbstractItemAttribute*> s;

    const auto visitor = [&s](const CT_AbstractItemAttribute* attribute) -> bool {
        s.insert(const_cast<CT_AbstractItemAttribute*>(attribute));
        return true;
    };

    visitItemAttributesForTypeInSelectedPossibilitiesOfInModel(itemType, inModel, visitor);

    return s.values();
}

CT_AbstractItemAttribute* CT_DefaultItemAttributeManager::firstItemAttributeFromInModel(const CT_InAbstractItemAttributeModel *inModel, const QString &itemType) const
{
    CT_AbstractItemAttribute* found = nullptr;

    const auto visitor = [&found](const CT_AbstractItemAttribute* attribute) -> bool {
        found = const_cast<CT_AbstractItemAttribute*>(attribute);
        return false;
    };

    visitItemAttributesForTypeInSelectedPossibilitiesOfInModel(itemType, inModel, visitor);

    return found;
}

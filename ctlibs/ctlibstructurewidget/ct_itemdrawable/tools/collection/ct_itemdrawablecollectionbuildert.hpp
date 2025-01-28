#include "ct_tools/itemdrawable/ct_itemdrawablecollectionbuildert.h"

template<typename... Type>
CT_ItemDrawableCollectionBuilderT<Type...>::CT_ItemDrawableCollectionBuilderT(bool mutuallyExclusive,
                                                                              bool excludeColorsOfDocument,
                                                                              bool excludeNormalsOfDocument) : SuperClass(mutuallyExclusive,
                                                                                                                          excludeColorsOfDocument,
                                                                                                                          excludeNormalsOfDocument)
{
}

template<typename... Type>
bool CT_ItemDrawableCollectionBuilderT<Type...>::buildFrom(const CT_VirtualAbstractStep *step, bool forceSearchModels)
{
    m_collection.clear();
    m_onlyFindModels = forceSearchModels;

    if(step == nullptr)
        return false;

    recursiveBuildFromStep(step, forceSearchModels);

    return true;
}

template<typename... Type>
const QList<CT_ItemDrawableCollectionHierarchyStep>& CT_ItemDrawableCollectionBuilderT<Type...>::collection() const
{
    return m_collection;
}

template<typename... Type>
CT_AbstractItemDrawableCollectionBuilder* CT_ItemDrawableCollectionBuilderT<Type...>::copy() const
{
    return new CT_ItemDrawableCollectionBuilderT<Type...>(*this);
}

template<typename... Type>
void CT_ItemDrawableCollectionBuilderT<Type...>::recursiveBuildFromStep(const CT_VirtualAbstractStep *step,
                                                                        bool findOnlyModels)
{
    CT_ItemDrawableCollectionHierarchyStep hi;
    hi.step = (CT_VirtualAbstractStep*)step;

    if(findOnlyModels) {
        const QList<CT_OutAbstractResultModel*> results = step->getOutResultModels();

        for(CT_OutAbstractResultModel* r : results) {
            CT_ItemDrawableCollectionHierarchyResult hir;
            hir.modelResult = r;

            recursiveBuildFromModels(QList<CT_OutAbstractModel *>() << r, hir, findOnlyModels);

            if(!hir.modelsCollection.isEmpty())
                hi.results.append(hir);
        }
    } else {
        const QList<CT_ResultGroup*> results = step->getOutResultList();

        for(CT_ResultGroup* result : results)
        {
            CT_ItemDrawableCollectionHierarchyResult hir;
            hir.result = result;

            recursiveBuildFromModels(QList<CT_OutAbstractModel *>() << static_cast<CT_OutAbstractModel*>(hir.result->model()->rootModel()), hir, findOnlyModels);

            if(!hir.collection.isEmpty())
                hi.results.append(hir);
        }
    }

    if(!hi.results.empty())
        m_collection.append(hi);

    const QList<CT_VirtualAbstractStep*> steps = step->getStepChildList();

    for(CT_VirtualAbstractStep* child : steps)
        recursiveBuildFromStep(child, findOnlyModels);
}

template<typename... Type>
void CT_ItemDrawableCollectionBuilderT<Type...>::recursiveBuildFromModels(QList<CT_OutAbstractModel *> models,
                                                                          CT_ItemDrawableCollectionHierarchyResult &hir,
                                                                          bool findOnlyModels)
{
    for(CT_OutAbstractModel* model : models)
    {
        if((model->result() != nullptr) || findOnlyModels)
        {
            CT_AbstractItemDrawable* item = nullptr;

            CT_OutAbstractSingularItemModel* itemModel = dynamic_cast<CT_OutAbstractSingularItemModel*>(model);
            CT_OutAbstractGroupModel* grModel = dynamic_cast<CT_OutAbstractGroupModel*>(model);

            if(itemModel != nullptr)
                item = itemModel->itemDrawable();
            else if(grModel != nullptr)
                item = grModel->group();

            if(canBeDynamicCastTo<CT_AbstractItemDrawable, Type...>(item))
            {
                if(grModel != nullptr)
                {
                    if(findOnlyModels) {
                        hir.modelsCollection.append(grModel);
                    } else {
                        CT_ResultGroupIterator it((CT_ResultGroup*)model->result(), grModel);

                        while(it.hasNext())
                            hir.collection.append((CT_AbstractSingularItemDrawable*)it.next());
                    }
                }
                else
                {
                    if(findOnlyModels) {
                        hir.modelsCollection.append(itemModel);
                    } else {
                        CT_ResultItemIterator it((CT_ResultGroup*)model->result(), itemModel);

                        while(it.hasNext())
                            hir.collection.append((CT_AbstractSingularItemDrawable*)it.next());
                    }
                }
            }

            // TODO : if it was an attribute !!

            recursiveBuildFromModels(model->childrensStaticCast<CT_OutAbstractModel>(), hir, findOnlyModels);
        }
    }
}

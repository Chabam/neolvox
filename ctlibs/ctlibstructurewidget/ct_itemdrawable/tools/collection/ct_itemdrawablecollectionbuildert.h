#ifndef CT_ITEMDRAWABLECOLLECTIONBUILDERT_H
#define CT_ITEMDRAWABLECOLLECTIONBUILDERT_H

#include "abstract/ct_abstractitemdrawablecollectionbuilder.h"

/**
 * @brief Build a collection of ItemDrawable that can be dynamically cast to <Type>. It will search in all steps recursively.
 *
 *        example : I wan't all CT_AbstractPointsAttributes of type Color and Scalar
 *
 *        // declare a builder that will search all items that can be dynamic casted to CT_AttributesColor or CT_AbstractAttributesScalar
 *        CT_ItemDrawableCollectionBuilderT<CT_AttributesColor, CT_AbstractAttributesScalar> builder (true);
 *
 *        // build the collection
 *        builder.buildFrom(rootStep);
 *
 *        // get the collection
 *        const QList<CT_ItemDrawableCollectionHierarchyStep> collection = builder.collection();
 *
 */
template<typename... Type>
class CT_ItemDrawableCollectionBuilderT : public CT_AbstractItemDrawableCollectionBuilder
{
    typedef CT_AbstractItemDrawableCollectionBuilder SuperClass;

public:
    /**
     * @brief Construct a builder
     * @param mutuallyExclusive : set to true if you want that results of this builder was mutually exclusive when it was checked
     * @param excludeColorsOfDocument : set to true if you want that when a result of this builder is checked the colors of document was unchecked
     * @param excludeNormalsOfDocument : set to true if you want that when a result of this builder is checked the normals of document was unchecked
     */
    CT_ItemDrawableCollectionBuilderT(bool mutuallyExclusive, bool excludeColorsOfDocument = false, bool excludeNormalsOfDocument = false);
    CT_ItemDrawableCollectionBuilderT(const CT_ItemDrawableCollectionBuilderT<Type...>& other) = default;

    /**
     * @brief Build the collection of ItemDrawable that type (staticType) match with template (ItemDrawable1, ItemDrawable2, ...).
     * @param step : the step where begin the search (algorithme go up in the hierarchy of step to find in results items searched)
     * @param forceSearchModels : True to search only models if step was not already launched. In this case the attribute
     *                         "modelsCollection" and "modelResult" of CT_ItemDrawableCollectionHierarchyResult will be filled.
     *
     *                         False to search only real ItemDrawable. In this case the attribute "collection" and "result" of CT_ItemDrawableCollectionHierarchyResult
     *                         will be filled.
     */
    bool buildFrom(const CT_VirtualAbstractStep *step, bool forceSearchModels) override;

    /**
     * @brief Get the collection builded
     */
    const QList<CT_ItemDrawableCollectionHierarchyStep>& collection() const override;

    CT_AbstractItemDrawableCollectionBuilder* copy() const override;

private:
    QList<CT_ItemDrawableCollectionHierarchyStep>   m_collection;
    bool                                            m_onlyFindModels;

    template <typename T, typename Last>
    bool canBeDynamicCastTo(const T* item)
    {
        return (dynamic_cast<const Last*>(item) != nullptr);
    }

    template <typename T, typename First, typename Second, typename... Rest>
    bool canBeDynamicCastTo(const T* item)
    {
        return canBeDynamicCastTo<T, First>(item) || canBeDynamicCastTo<T, Second, Rest...>(item);
    }

    /**
     * @brief Recursively search items that can be casted to Type and add it to the collection
     * @param step : the current step that must be used to search items and call recursively the method with it childrens
     * @param findOnlyModels : set to true if you want only search models per example if steps has not be launched yet
     */
    void recursiveBuildFromStep(const CT_VirtualAbstractStep* step, bool findOnlyModels);

    /**
     * @brief Recursively search items in models that can be casted to Type and add it to the collection
     * @param models : list of models to use to get itemdrawable and verify if it can be casted to Type. If true the model will be added to the collection
     * @param hir : the result (collection) to use to add all models or real items
     * @param findOnlyModels : set to true if you want only search models per example if steps has not be launched yet
     */
    void recursiveBuildFromModels(QList<CT_OutAbstractModel*> models, CT_ItemDrawableCollectionHierarchyResult &hir, bool findOnlyModels);
};

#include "ct_itemdrawablecollectionbuildert.hpp"

#endif // CT_ITEMDRAWABLECOLLECTIONBUILDERT_H

#ifndef CT_OUTRESULTMODELGROUPCOPY_H
#define CT_OUTRESULTMODELGROUPCOPY_H

#include <QSet>

#include "ct_model/outModel/abstract/ct_outabstractresultmodelgroup.h"
#include "ct_model/inModel/abstract/def_ct_abstractgroupmodelin.h"
#include "ct_model/inModel/abstract/def_ct_abstractitemdrawablemodelin.h"
#include "ct_model/outModel/abstract/def_ct_abstractitemdrawablemodelout.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"

class CT_InResultModelGroupToCopy;

/**
 * @brief Use this class if you want to create a result that was a COPY of a possibility of
 *        an input result. In other terms : if you want to create a copy of a result of a parent step use this class.
 */
class CTLIBMODELS_EXPORT CT_OutResultModelGroupCopy : public CT_OutAbstractResultModelGroup
{
    typedef CT_OutAbstractResultModelGroup SuperClass;

public:
    /**
     * @brief Construct an output model that represent a result group that will be copied
     * @param inResultModel : the input result model that has only one possibility and will be used to get the output result model to copy.
     * @param resultName: the name that will be set to the created result. Empty by default.
     * @param modelDisplayableName : a displayable name for the model (used in gui for user information). Empty by default.
     * @param modelShortDescription : a short description of the result you search. Empty by default.
     */
    CT_OutResultModelGroupCopy(const CT_InResultModelGroupToCopy* inResultModel,
                               const QString& resultName = "",
                               const QString& modelDisplayableName = "",
                               const QString& modelShortDescription = "");
    ~CT_OutResultModelGroupCopy();

    /**
     * @brief Copy constructor
     * @warning The parent model will not be copied !
     */
    CT_OutResultModelGroupCopy(const CT_OutResultModelGroupCopy& other);

    /**
     * @brief Redefined to change the root group of the copy.
     * @param rootGroup : the new root group or nullptr if you want to remove the current root group.
     */
    void setRootGroup(DEF_CT_AbstractGroupModelOut* rootGroup) override;

    /**
     * @brief Redefined to returns the root group of the copy
     */
    DEF_CT_AbstractGroupModelOut* rootGroup() const override;

    /**
     * @brief Add a new output group to output groups of this result model if it was in selected possibilities
     *        of the input group (inParentGroup). The unique index of output groups from possibilities will be
     *        used to find output groups of this result model. For each output groups found in this model, a
     *        copy of the "childToCopy" will be created and added to it and to the "modelsCreated" collection.
     *
     * @param inParentGroup : the input group model to use to get unique index of output group model in his possibilities.
     * @param childToCopy : the prototype of the output group model that will be copied to each output groups parent.
     * @param modelsCreated : a collection that will contains all copy of the "childGroupToCopy" created and added. The
     *                        collection will not be cleared before we add copies.
     */
    template<class GroupToCopyType, typename CollectionType>
    void addGroup(const DEF_CT_AbstractGroupModelIn* inParentGroup,
                  const GroupToCopyType* childToCopy,
                  CollectionType& modelsCreated) {

        const auto visitor = [&](CT_OutAbstractModel* outModelThatCanBeModified) {
            // make a new copy of the element
            GroupToCopyType* elementCopy = static_cast<GroupToCopyType*>(childToCopy->copy());

            // add it to the out model (parent that can be modified)
            static_cast<DEF_CT_AbstractGroupModelOut*>(outModelThatCanBeModified)->addGroup(elementCopy);

            // and in the collection of models created
            modelsCreated.append(elementCopy);
        };

        internalVisitOutModelThatCanBeModified(inParentGroup,
                                               visitor);
    }

    /**
     * @brief Remove all group that was in selected possibilities of the input model specified
     * @param inGroupToRemove : the input model to use to get possibilities
     */
    void removeGroup(const DEF_CT_AbstractGroupModelIn* inGroupToRemove);

    /**
     * @brief Add a new output item to output groups of this result model if it was in selected possibilities
     *        of the input group (inParentGroup). The unique index of output groups from possibilities will be
     *        used to find output groups of this result model. For each output groups found in this model, a
     *        copy of the "childToCopy" will be created and added to it and to the "modelsCreated" collection.
     *
     * @param inParentGroup : the input group model to use to get unique index of output group model in his possibilities.
     * @param childToCopy : the prototype of the output item model that will be copied to each output groups parent.
     * @param modelsCreated : a collection that will contains all copy of the "childToCopy" created and added. The
     *                        collection will not be cleared before we add copies.
     */
    template<class ItemToCopyType, typename CollectionType>
    void addItem(const DEF_CT_AbstractGroupModelIn* inParentGroup,
                 const ItemToCopyType* childToCopy,
                 CollectionType& modelsCreated) {

        const auto visitor = [&](CT_OutAbstractModel* outModelThatCanBeModified) {
            // make a new copy of the element
            ItemToCopyType* elementCopy = static_cast<ItemToCopyType*>(childToCopy->copy());

            // add it to the out model (parent that can be modified)
            static_cast<DEF_CT_AbstractGroupModelOut*>(outModelThatCanBeModified)->addItem(elementCopy);

            // and in the collection of models created
            modelsCreated.append(elementCopy);
        };


        internalVisitOutModelThatCanBeModified(inParentGroup,
                                               visitor);
    }

    /**
     * @brief Remove all item that was in selected possibilities of the input model specified
     * @param inItemToRemove : the input model to use to get possibilities
     */
    void removeItem(const DEF_CT_AbstractItemDrawableModelIn* inItemToRemove);

    /**
     * @brief Add a new output item attribute to output items of this result model if it was in selected possibilities
     *        of the input item (inParentItem). The unique index of output items from possibilities will be
     *        used to find output items of this result model. For each output items found in this model, a
     *        copy of the "childToCopy" will be created and added to it and to the "modelsCreated" collection.
     *
     * @param inParentItem : the input item model to use to get unique index of output item model in his possibilities.
     * @param childToCopy : the prototype of the output item attribute model that will be copied to each output items parent.
     * @param modelsCreated : a collection that will contains all copy of the "childToCopy" created and added. The
     *                        collection will not be cleared before we add copies.
     */
    template<class ItemAttributeToCopyType, typename CollectionType>
    void addItemAttribute(const DEF_CT_AbstractItemDrawableModelIn* inParentItem,
                          const ItemAttributeToCopyType* childToCopy,
                          CollectionType& modelsCreated) {

        const auto visitor = [&](CT_OutAbstractModel* outModelThatCanBeModified) {
            // make a new copy of the element
            ItemAttributeToCopyType* elementCopy = static_cast<ItemAttributeToCopyType*>(childToCopy->copy());

            // add it to the out model (parent that can be modified)
            static_cast<DEF_CT_AbstractItemDrawableModelOut*>(outModelThatCanBeModified)->addItemAttribute(elementCopy);

            // and in the collection of models created
            modelsCreated.append(elementCopy);
        };


        internalVisitOutModelThatCanBeModified(inParentItem,
                                               visitor);
    }

    /**
     * @brief Recursively search in the variable m_inResultModel the child that was a copy of the specified original model
     * @param originalModel : the original model to compare with the "originalModel" returned by child in this input result model
     * @return The copied model if founded or the original model if it was in this result model. nullptr if not founded.
     */
    CT_OutAbstractModel* recursiveSearchTheModelThatWasACopiedModelFromThisOriginalModel(const CT_OutAbstractModel* originalModel) const;

    /**
     * @brief Redefined to create a copy of the result and not a new result
     */
    IResultForModel* createResult() override;

    /**
     * @brief Returns a copy of this model
     */
    CT_OutAbstractResultModel* copy() const override;

private:
    CT_InResultModelGroupToCopy*            m_inResultModel;
    DEF_CT_AbstractGroupModelOut*           m_rootGroupForModification;

    /**
     * @brief Recursively search in the variable m_inResultModel the child that was a copy of the specified original model
     * @param originalModel : the original model to compare with the "originalModel" returned by child in this input result model
     * @return The copied model if founded or the original model if it was in this result model. nullptr if not founded.
     */
    CT_InAbstractModel* recursiveSearchTheInModelThatWasACopiedModelFromThisOriginalInModel(const CT_InAbstractModel* originalModel) const;

    /**
     * @brief Call this method if you have an input model and you want to visit all out model (modifiable) that correspond to output model inside selected
     *        possibilities of the specified input model.
     * @param inModel : the input model to use to get possibilities and out model unique index
     * @param visitor : the visitor that will receive a pointer to a CT_OutAbstractModel (the model that was modifiable)
     */
    template<class InModelType, typename Visitor>
    void internalVisitOutModelThatCanBeModified(const InModelType* inModel,
                                                const Visitor& visitor)  {
        // here the inParent can be an original model that doesn't have possibilities but the copied
        // model has possibilities. So we must found the model that was a copy from this original model. Look
        // the class CT_InAbstractResultModel for more information about the copy of models when search possibilities.
        const CT_InAbstractModel* inModelForSearch = recursiveSearchTheInModelThatWasACopiedModelFromThisOriginalInModel(inModel);

        MODELS_ASSERT(inModelForSearch != nullptr);
        if (inModelForSearch == nullptr) {qDebug() << "CT_OutResultModelGroupCopy::internalVisitOutModelThatCanBeModified" << ", " <<  "inModelForSearch == nullptr"; return;}

        const CT_InStdModelPossibilitySelectionGroup* selectionGroup = inModelForSearch->possibilitiesGroup();

        QSet<CT_OutAbstractModel*> outModelsToFind;
        outModelsToFind.reserve(selectionGroup->nPossibilitySelected());

        // get from possibilities of the input group all unique index to find in this result
        const auto visitorP = [&outModelsToFind](const CT_InStdModelPossibility* possibility) -> bool {
            outModelsToFind.insert(possibility->outModel()->recursiveOriginalModel());
            return true;
        };

        selectionGroup->visitSelectedPossibilities(visitorP);

        const int nToFind = outModelsToFind.size();

        if(nToFind == 0)
            return;

        int nFound = 0;

        // visit all childrens (recursively) of the output result model
        // and find models that unique index is in the QSet.
        const auto visitorC = [&outModelsToFind, &nFound, &nToFind, &visitor](const CT_OutAbstractModel* outModel) -> bool {

            // if it is in the QSet
            if(outModelsToFind.contains(outModel->recursiveOriginalModel())) {

                // visit it
                visitor(const_cast<CT_OutAbstractModel*>(outModel));

                ++nFound;

                // stop when all childrens has been found
                if(nFound == nToFind)
                    return false;
            }

            return true;
        };

        recursiveVisitOutChildrens(visitorC);
    }
};

#endif // CT_OUTRESULTMODELGROUPCOPY_H

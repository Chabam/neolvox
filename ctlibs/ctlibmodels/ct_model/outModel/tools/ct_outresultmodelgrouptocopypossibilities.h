#ifndef CT_OUTRESULTMODELGROUPTOCOPYPOSSIBILITIES_H
#define CT_OUTRESULTMODELGROUPTOCOPYPOSSIBILITIES_H

#include "ct_model/outModel/ct_outresultmodelgroupcopy.h"
#include "ct_model/inModel/abstract/def_ct_abstractgroupmodelin.h"

#include <QList>

/**
 * @brief When a developper of a plugin declare that he will copy the input result it is possible
 *        that it has set the maximum number of possibility to be > 1. So it is possible that the
 *        user has selected multiple input result that means the algorithm must copy multiple
 *        input result. So when he had a new group or item or xxx in one output result (that
 *        it was a copy of the input) it means that the new element must be added not to one
 *        output result but multiple output result !
 *
 *        So this class is used to add a new element or remove an element to all output
 *        result model (that was a copy of an input result model with one or multiple possibility).
 */
class CTLIBMODELS_EXPORT CT_OutResultModelGroupToCopyPossibilities
{
public:
    using ModelType = CT_OutResultModelGroupCopy;

    CT_OutResultModelGroupToCopyPossibilities();
    ~CT_OutResultModelGroupToCopyPossibilities();

    /**
     * @brief Return the number of result model added to this tool
     */
    int nResultModels() const;

    /**
     * @brief Visit all output result model added to this tool
     * @param visitor : a lambda expression per example that return a bool (true if must continue the visit) and receive a const pointer to a CT_OutResultModelGroupCopy
     * @return True if no results has been visited, otherwise the returned value of the visitor
     */
    template<typename Visitor>
    bool visitOutResultModelCopies(const Visitor& visitor) const {
        for(const CT_OutResultModelGroupCopy* outResultModel : m_models) {
            if(!visitor(outResultModel))
                return false;
        }

        return true;
    }

    /**
     * @brief Add a copy of the specified output root group model to all result model in this tool.
     *        It is possible that this tool contains more that one result so this will do the staff for each result model.
     *
     * @param outRootGroupToCopy : the model (prototype) to copy for set as the root group to all result model
     * @param allOutModelsCreated : a collection that will contains all copy that was created and set to parent model
     */
    template<class GroupToCopyType, class CollectionType>
    void setRootGroup(const GroupToCopyType* outRootGroupToCopy,
                      CollectionType& allOutModelsCreated)
    {
        MODELS_ASSERT(outRootGroupToCopy != nullptr);
        if (outRootGroupToCopy == nullptr) {qDebug() << "CT_OutResultModelGroupToCopyPossibilities::setRootGroup" << ", " <<  "outRootGroupToCopy == nullptr"; return;}

        for(CT_OutResultModelGroupCopy* outResultModel : m_models) {
            GroupToCopyType* rootGroupCopy = static_cast<GroupToCopyType*>(outRootGroupToCopy->copy());
            outResultModel->setRootGroup(rootGroupCopy);
            allOutModelsCreated.append(rootGroupCopy);
        }
    }

    /**
     * @brief Remove the root group to all result model in this tool
     */
    void removeRootGroup();

    /**
     * @brief Add a copy of the specified output group model to all output parent group model that was in possibility selected of the input model.
     *        It is possible that this tool contains more that one result so this will do the staff for each result model.
     *
     * @param inParentGroup : the input model to use to get possibilities
     * @param outChildGroupToCopy : the model (prototype) to copy for set as the child group to all parent group model
     * @param allOutModelsCreated : a collection that will contains all copy that was created and set to parent model
     */
    template<class GroupToCopyType, class CollectionType>
    void addGroup(const DEF_CT_AbstractGroupModelIn* inParentGroup,
                  const GroupToCopyType* outChildGroupToCopy,
                  CollectionType& allOutModelsCreated)
    {
        MODELS_ASSERT((inParentGroup != nullptr) && (outChildGroupToCopy != nullptr));
        if ((inParentGroup == nullptr) || (outChildGroupToCopy == nullptr)) {qDebug() << "CT_OutResultModelGroupToCopyPossibilities::addGroup" << ", " <<  "(inParentGroup == nullptr) || (outChildGroupToCopy == nullptr)"; return;}

        for(CT_OutResultModelGroupCopy* outResultModel : m_models) {
            outResultModel->addGroup(inParentGroup,
                                     outChildGroupToCopy,
                                     allOutModelsCreated);
        }
    }

    /**
     * @brief Remove all group that was in selected possibilities of the input model specified
     * @param inGroupToRemove : the input model to use to get possibilities
     */
    void removeGroup(const DEF_CT_AbstractGroupModelIn* inGroupToRemove);

    /**
     * @brief Add a copy of the specified output item model to all output parent group model that was in possibility selected of the input model.
     *        It is possible that this tool contains more that one result so this will do the staff for each result model.
     *
     * @param inParentGroup : the input model to use to get possibilities
     * @param outChildItemToCopy : the model (prototype) to copy for set as the child item to all parent group model
     * @param allOutModelsCreated : a collection that will contains all copy that was created and set to parent model
     */
    template<class ItemToCopyType, class CollectionType>
    void addItem(const DEF_CT_AbstractGroupModelIn* inParentGroup,
                 const ItemToCopyType* outChildItemToCopy,
                 CollectionType& allOutModelsCreated)
    {
        MODELS_ASSERT((inParentGroup != nullptr) && (outChildItemToCopy != nullptr));
        if ((inParentGroup == nullptr) || (outChildItemToCopy == nullptr)) {qDebug() << "CT_OutResultModelGroupToCopyPossibilities::addItem" << ", " <<  "(inParentGroup == nullptr) || (outChildItemToCopy == nullptr)"; return;}

        for(CT_OutResultModelGroupCopy* outResultModel : m_models) {
            outResultModel->addItem(inParentGroup,
                                    outChildItemToCopy,
                                    allOutModelsCreated);
        }
    }

    /**
     * @brief Remove all item that was in selected possibilities of the input model specified
     * @param inItemToRemove : the input model to use to get possibilities
     */
    void removeItem(const DEF_CT_AbstractItemDrawableModelIn* inItemToRemove);

    /**
     * @brief Add a copy of the specified output item attribute model to all output parent item model that was in possibility selected of the input model.
     *        It is possible that this tool contains more that one result so this will do the staff for each result model.
     *
     * @param inParentItem : the input model to use to get possibilities
     * @param outChildItemAttributeToCopy : the model (prototype) to copy for set as the child item attribute to all parent item model
     * @param allOutModelsCreated : a collection that will contains all copy that was created and set to parent model
     */
    template<class ItemAttributeToCopyType, class CollectionType>
    void addItemAttribute(const DEF_CT_AbstractItemDrawableModelIn* inParentItem,
                          const ItemAttributeToCopyType* outChildItemAttributeToCopy,
                          CollectionType& allOutModelsCreated)
    {
        MODELS_ASSERT((inParentItem != nullptr) && (outChildItemAttributeToCopy != nullptr));
        if ((inParentItem == nullptr) || (outChildItemAttributeToCopy == nullptr)) {qDebug() << "CT_OutResultModelGroupToCopyPossibilities::addItemAttribute" << ", " <<  "(inParentItem == nullptr) || (outChildItemAttributeToCopy == nullptr)"; return;}

        for(CT_OutResultModelGroupCopy* outResultModel : m_models) {
            outResultModel->addItemAttribute(inParentItem,
                                             outChildItemAttributeToCopy,
                                             allOutModelsCreated);
        }
    }

    /**
     * @brief Call to add a new result model that will be used to made modification on it
     * @warning Not intented for direct use by plugin developper
     */
    void addResulModel(CT_OutResultModelGroupCopy* rModel);

private:
    QList<CT_OutResultModelGroupCopy*>  m_models;
};

#endif // CT_OUTRESULTMODELGROUPTOCOPYPOSSIBILITIES_H

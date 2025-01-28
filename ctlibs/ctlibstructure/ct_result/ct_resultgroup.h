#ifndef CT_RESULTGROUP_H
#define CT_RESULTGROUP_H

#include "ct_item/abstract/ct_abstractitem.h"
#include "ct_result/abstract/ct_abstractresult.h"
#include "ct_itemdrawable/ct_standarditemgroup.h"
#include "ct_tools/ct_containert.h"

#include <QMutex>

/**
 * @brief Represent a result that contains root groups
 */
class CTLIBSTRUCTURE_EXPORT CT_ResultGroup : public CT_AbstractResult, public IResultGroupForModel
{
    Q_OBJECT
    using SuperClass = CT_AbstractResult;

public:
    using const_iterator = CT_StandardItemGroup::GroupContainerType::const_iterator;
    using iterator = CT_StandardItemGroup::GroupContainerType::iterator;

    CT_ResultGroup();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Displayable name
     *
     *        What is initialized differently :
     *          - Parent step is set to nullptr
     *          - Result model is set to nullptr
     *          - Number of items visible is set to 0
     *          - Clear from memory progress value is set to represent a result cleared
     *          - Root groups was not copied
     */
    CT_ResultGroup(const CT_ResultGroup& other);
    ~CT_ResultGroup();

    /**
     * @brief Returns a unique type for a result
     * @overload Overloaded to return a unique type
     */
    QString type() const override;

    /**
     * @brief Returns a unique type for a result
     * @overload Overloaded to return a unique type
     */
    static QString staticType();

    /**
     * @brief Returns true if the result is empty (if some root group must be removed later it will not be considered here)
     */
    bool isEmpty() const override;

    /**
     * @brief Returns the number of root group (only root group that must be keeped)
     */
    int nRootGroups() const;

    /**
     * @brief Returns the number of root group (only root group that must be removed later)
     */
    int nRootGroupsToBeRemovedLater() const;

    /**
     * @brief Add a group to the result.
     * @param outRootGroupHandle : the handle of the root group
     * @param rootGroup : the root group to add (life in memory of this group will be managed by this result)
     * @warning There is no verification made to check that this group can be added or not to this result. Be sure what you do.
     */
    template<typename OutHandleType>
    void addRootGroup(const OutHandleType& outRootGroupHandle, CT_StandardItemGroup* rootGroup) {
        Q_ASSERT(outRootGroupHandle.nModels() == 1);
        if (outRootGroupHandle.nModels() != 1) {qDebug() << "CT_ResultGroup::addRootGroup" << ", " << "outRootGroupHandle.nModels() != 1";}

        addRootGroupWithOutModel(outRootGroupHandle.firstModel(), rootGroup);
    }

    /**
     * @brief Add a group to the result.
     * @param outRootGroupHandle : the handle of the root group
     * @param rootGroups : a collection of root group to add (life in memory of groups will be managed by this result)
     * @warning There is no verification made to check that this group can be added or not to this result. Be sure what you do.
     */
    template<typename OutHandleType>
    void addRootGroups(const OutHandleType& outRootGroupHandle, const QList<CT_StandardItemGroup*>& rootGroups) {
        Q_ASSERT(outRootGroupHandle.nModels() == 1);
        if (outRootGroupHandle.nModels() != 1) {qDebug() << "CT_ResultGroup::outRootGroupHandle" << ", " << "outRootGroupHandle.nModels() != 1";}

        addRootGroupsWithOutModel(outRootGroupHandle.firstModel(), rootGroups);
    }

    /**
     * @brief Add a root group to the result.
     * @param outModel : the model to set to the root group and to use to add it in the collection of root groups
     * @param rootGroup : the root group to add (life in memory of this group will be managed by this result)
     * @warning There is no verification made to check that this group can be added or not to this result. Be sure what you do.
     */
    void addRootGroupWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_StandardItemGroup* rootGroup);

    /**
     * @brief Add a list of root groups to the list of root groups of this result
     * @param outModel : the model to set to all of root group of the collection and to use to add it in the collection of root groups
     * @param rootGroups : a collection of root group to add (life in memory of groups will be managed by this result)
     * @warning There is no verification made to check that groups can be added or not to this result. Be sure what you do.
     */
    void addRootGroupsWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, const QList<CT_StandardItemGroup*>& rootGroups);

    /**
     * @brief Returns a STL-style iterator pointing to the first element in the collection of root groups that must be keeped.
     */
    iterator begin();

    /**
     * @brief Returns a STL-style iterator pointing to the imaginary element after the element item in the collection of root groups that must be keeped.
     */
    iterator end();

    /**
     * @brief Returns a const STL-style iterator pointing to the first element in the collection of root groups that must be keeped.
     */
    const_iterator constBegin() const;

    /**
     * @brief Returns a const STL-style iterator pointing to the imaginary element after the element item in the collection of root groups that must be keeped.
     */
    const_iterator constEnd() const;

    /**
     * @brief Visit childrens. Redefined to visit root groups.
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    bool visitChildrens(const ChildrensVisitor& visitor) const override;

    /**
     * @brief Returns the number of root groups
     */
    int nChildrens() const override;

    /**
     * @brief Returns the collection of childrens that use the specified out model. You can use the begin and end method to iterate
     *        over child inside the collection
     * @param outModel : the out model of child
     * @return An object that you can use to iterate over childrens
     */
    CT_AbstractItem::ChildrensCollection childrensThatUseOutModel(const CT_OutAbstractModel* outModel) const;

    /**
     * @brief When the step finish is compute this method is called.
     * @overload Overload to remove all elements that have been set to be removed later
     */
    void setComplete() override;

    /**
     * @brief Returns a copy of this model
     */
    CT_AbstractResult* copy(const CT_OutAbstractResultModel* newModelToUse) const override;

    /**
     * @brief Returns the tool for model
     */
    IPrototypeToolForModel* prototypeToolForModel() const override;

    /**
     * @brief Returns the tool for model
     */
    IResultToolForModel* resultToolForModel() const override;

protected:
    /**
     * @brief Clear from memory all root groups of this result
     */
    void internalClearFromMemory() override;

    /**
     * @brief Create and return a qt style iterator to iterate over childrens (groups or items or etc...) that use the specified out model
     * @param outModel : the out model of childrens
     * @return A new qt style iterator to iterate over childrens (groups or items or etc...) that use the specified out model
     */
    IChildrensIteratorQtStylePtr createQtStyleIteratorForChildrensThatUseOutModel(const CT_OutAbstractModel* outModel) const override;

private:
    using GroupContainerType = CT_StandardItemGroup::GroupContainerType;

    class GroupIterator : public CT_AbstractItem::IChildrensIteratorQtStyle {
    public:
        using GroupContainerType = CT_ResultGroup::GroupContainerType;

        GroupIterator(GroupContainerType::iterator begin, GroupContainerType::iterator end) : m_begin(begin), m_end(end) {}

        bool hasNext() const override { return m_begin != m_end; }

        CT_AbstractItem* next() override {
            CT_AbstractItem* c = (*m_begin);
            m_begin++;
            return c;
        }

        IChildrensIteratorQtStyle* copy() const override { return new GroupIterator(m_begin, m_end); }

    private:
        GroupContainerType::iterator m_begin;
        GroupContainerType::iterator m_end;
    };


    CT_StandardItemGroup::LockAccessTool    m_lockAccessTool;

    /**
     * @brief Collection of root groups added to this group
     */
    GroupContainerType*                     m_rootGroups;

    /**
     * @brief Remove all root groups from this result
     */
    void clearRootGroups();
};

#endif // CT_RESULTGROUP_H

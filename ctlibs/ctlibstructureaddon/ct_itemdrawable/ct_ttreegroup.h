#ifndef CT_TTREEGROUP_H
#define CT_TTREEGROUP_H

#include "ct_itemdrawable/ct_tnodegroup.h"

/**
 * @brief A class that represent a topology tree. You must only set a root CT_TNodeGroup in this
 *        tree.
 *
 *        The model must be implemented like this :
 *
 *        CT_TTreeGroup (must contains all different types that you want to add in the topology)
 *          |
 *          |-- CT_TNodeGroup (first type, per example Axis)
 *          |       |
 *          |       |-- CT_MeshModel (a mesh model or a CT_AbstractSingularItemDrawable that represent the shape, not obligatory)
 *          |       |-- CT_MetricT<?> (one or multiple metric that represent a attribut if you want to add a attribute in the topology, not obligatory)
 *          |       |-- etc...
 *          |
 *          |-- CT_TNodeGroup (second type, per example Internode)
 *          |
 *          |-- etc ... (other types)
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_TTreeGroup : public CT_StandardItemGroup
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_TTreeGroup, CT_StandardItemGroup, Tree group)

    using SuperClass = CT_StandardItemGroup;

public:
    CT_TTreeGroup();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - ID
     *          - Pointer of base and alternative draw manager
     *          - Pointer of context
     *          - Removed later flags
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     *          - Parent container is set to nullptr
     *          - Childrens (root node or items) was not copied
     */
    CT_TTreeGroup(const CT_TTreeGroup& other);

    /**
     * @brief Visit the root node if not nullptr
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    bool visitChildrensForTreeView(const ChildrensVisitor& visitor) const override;

    /**
     * @brief Set root group. Previous root group will be removed.
     * @param outGroupHandle : the handle of the root group
     * @param group : the group to add (life in memory of this group will be managed by this group)
     * @warning There is no verification made to check that this group can be added or not to this group. Be sure what you do.
     */
    template<typename OutHandleType>
    bool setRootNode(const OutHandleType& outGroupHandle, CT_TNodeGroup* root) {
        QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

        Q_ASSERT(model() != nullptr);
        if (model() == nullptr) {qDebug() << "CT_TTreeGroup::setRootNode" << ", " << "model() == nullptr";}

        // the handle can have multiple models if it was created with a result copy so we must get the model
        // that his parent match with the model of this group
        const DEF_CT_AbstractGroupModelOut* outModelToUse = outGroupHandle.findAbstractModelWithParent(model());

        // now we can add the group with the right model
        return setRootNodeWithOutModel(outModelToUse, root);
    }

    /**
     * @brief Set root group. Previous root group will be removed.
     * @param outModel : the model to set to the root group and to use to add it in the collection of groups
     * @param root : the root group to set (life in memory of this group will be managed by this group)
     * @warning There is no verification made to check that this group can be added or not to this group. Be sure what you do.
     */
    bool setRootNodeWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* root);

    template<typename OutHandleType>
    bool addSuccessor(CT_TNodeGroup* current, const OutHandleType& sucessorGroupHandle, CT_TNodeGroup* successor) {
        return current->setSuccessor(sucessorGroupHandle, successor);
    }

    bool addSuccessorWithOutModel(CT_TNodeGroup* current, const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* successor);

    template<typename OutHandleType>
    bool addComponent(CT_TNodeGroup* complex, const OutHandleType& componentGroupHandle, CT_TNodeGroup* component) {
        return complex->addComponent(componentGroupHandle, component);
    }

    bool addComponentWithOutModel(CT_TNodeGroup* complex, const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* component);

    template<typename OutHandleType>
    bool addBranch(CT_TNodeGroup* bearer, const OutHandleType& sonGroupHandle, CT_TNodeGroup* son) {
        return bearer->addBranch(sonGroupHandle, son);
    }

    bool addBranchWithOutModel(CT_TNodeGroup* bearer, const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* son);

    CT_TNodeGroup* rootNode() const;

    /**
     * @brief Returns a copy of this group that will use the new model and the new result. If some child groups
     *        or items was not present in collection of children of the new model they will not be copied
     * @param model : the new model to use
     * @param result : the new result to set to the copy
     * @return A copy of this group but if some child groups or items was not present in collection of children
     *         of the new model they will not be copied
     */
    CT_AbstractItemDrawable* copy(const CT_OutAbstractItemModel* model, const CT_AbstractResult* result) const override;

private:
    CT_TNodeGroup   *m_rootNode;

    CT_TNodeGroup* recursiveCopyNodes(const CT_OutAbstractGroupModel *newModel,
                                      const CT_AbstractResult *result,
                                      CT_TTreeGroup *group,
                                      CT_TNodeGroup *node,
                                      const QHash<int, CT_OutAbstractGroupModel*>& modelsMap) const;

protected:
    friend class CT_TNodeGroup;

    /**
     * @brief Add the node to the structure
     */
    bool addNode(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup *n);

    /**
     * @brief Remove the node from the structure
     * @warning if the node has children (successor, etc...) you must call this method for all childrens !!!
     */
    bool removeNode(CT_TNodeGroup *n);
};

#endif // CT_TTREEGROUP_H

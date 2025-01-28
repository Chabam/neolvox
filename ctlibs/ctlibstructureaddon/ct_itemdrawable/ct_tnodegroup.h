#ifndef CT_TNODEGROUP_H
#define CT_TNODEGROUP_H

#include "ct_itemdrawable/ct_standarditemgroup.h"
#include "ctlibstructureaddon_global.h"

class CT_TTreeGroup;

/**
 * @brief A class that you can use with a CT_TTreeGroup to create a topology.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_TNodeGroup : public CT_StandardItemGroup
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_TNodeGroup, CT_StandardItemGroup, Node group)

    using SuperClass = CT_StandardItemGroup;

public:
    CT_TNodeGroup();

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
     *          - Childrens (items, sucessor, components, branches) was not copied (you must copy the CT_TTreeGroup that will set the structure)
     */
    CT_TNodeGroup(const CT_TNodeGroup& other);

    /**
     * @brief Visit childrens but inly for tree view
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    bool visitChildrensForTreeView(const ChildrensVisitor& visitor) const override;

    /**
     * @brief Set the successor of the node
     * @warning if a successor already exist it will be removed and can not be used after (it will be deleted from memory at any moment)
     */
    template<typename OutHandleType>
    bool setSuccessor(const OutHandleType& sucessorGroupHandle, CT_TNodeGroup *successor) {
        QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

        Q_ASSERT(model() != nullptr);
        if (model() == nullptr) {qDebug() << "CT_TNodeGroup::setSuccessor" << ", " << "model() == nullptr";}

        // the handle can have multiple models if it was created with a result copy so we must get the model
        // that his parent match with the model of this group
        const DEF_CT_AbstractGroupModelOut* outModelToUse = sucessorGroupHandle.findAbstractModelWithParent(treeModel());

        // now we can add the group with the right model
        return setSuccessorWithOutModel(outModelToUse, successor);
    }

    bool setSuccessorWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* successor);

    /**
     * @brief Set the root component or if already exist set the successor to the end of successor of the list
     */
    template<typename OutHandleType>
    bool addComponent(const OutHandleType& componentGroupHandle, CT_TNodeGroup *component) {
        QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

        Q_ASSERT(model() != nullptr);
        if (model() == nullptr) {qDebug() << "CT_TNodeGroup::addComponent" << ", " << "model() == nullptr";}

        // the handle can have multiple models if it was created with a result copy so we must get the model
        // that his parent match with the model of this group
        const DEF_CT_AbstractGroupModelOut* outModelToUse = componentGroupHandle.findAbstractModelWithParent(treeModel());

        // now we can add the group with the right model
        return addComponentWithOutModel(outModelToUse, component);
    }

    bool addComponentWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup *component);

    /**
     * @brief Add a branch to the list
     */
    template<typename OutHandleType>
    bool addBranch(const OutHandleType& sonGroupHandle, CT_TNodeGroup *son) {
        QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

        Q_ASSERT(model() != nullptr);
        if (model() == nullptr) {qDebug() << "CT_TNodeGroup::addBranch" << ", " << "model() == nullptr";}

        // the handle can have multiple models if it was created with a result copy so we must get the model
        // that his parent match with the model of this group
        const DEF_CT_AbstractGroupModelOut* outModelToUse = sonGroupHandle.findAbstractModelWithParent(treeModel());

        // now we can add the group with the right model
        return addBranchWithOutModel(outModelToUse, son);
    }

    bool addBranchWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup *son);

    /**
     * @brief Remove the component passed in parameter
     * @param recursively : if true all successor of this component will be removed too
     * @warning removed component can not be used after (it will be deleted from memory at any moment)
     */
    bool removeComponent(CT_TNodeGroup *component, bool recursively = true);

    /**
     * @brief Remove the branch passed in parameter
     * @warning removed branch can not be used after (it will be deleted from memory at any moment)
     */
    bool removeBranch(CT_TNodeGroup *son);

    /**
     * @brief Returns the ancestor of this node
     */
    CT_TNodeGroup* ancestor() const;

    /**
     * @brief Returns the successor of this node
     */
    CT_TNodeGroup* successor() const;

    /**
     * @brief Returns the complex of this node
     */
    CT_TNodeGroup* complex() const;

    /**
     * @brief Returns the root component of this node
     */
    CT_TNodeGroup* rootComponent() const;

    /**
     * @brief Returns the bearer of this node
     */
    CT_TNodeGroup* bearer() const;

    /**
     * @brief Returns branches of this node
     */
    const QList<CT_TNodeGroup*>& branches() const;

    /**
     * @brief Returns the number of component of this node
     */
    size_t nComponent() const;

    CT_ITEM_COPY_IMP(CT_TNodeGroup)

private:
    CT_TTreeGroup           *m_tree;

    CT_TNodeGroup           *m_ancestor;
    CT_TNodeGroup           *m_successor;

    CT_TNodeGroup           *m_complex;
    CT_TNodeGroup           *m_rootComponent;
    CT_TNodeGroup           *m_lastComponent;

    CT_TNodeGroup           *m_bearer;
    QList<CT_TNodeGroup*>   m_branches;

    QList<CT_TNodeGroup*>   m_iterateList;

protected:
    CT_TTreeGroup* tree() const;
    CT_OutAbstractModel* treeModel() const;

    void internalSetSuccessor(CT_TNodeGroup *successor);

    void setAncestor(CT_TNodeGroup *o);
    void setComplex(CT_TNodeGroup *o);
    void setBearer(CT_TNodeGroup *o);

    bool addNode(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* n);
    bool recursiveRemoveNode(CT_TNodeGroup *n);

    friend class CT_TTreeGroup;
    void setTopologyTree(CT_TTreeGroup *tree);
};

#endif // CT_TNODEGROUP_H

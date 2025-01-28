#include "ct_ttreegroup.h"

CT_TYPE_IMPL_INIT_MACRO(CT_TTreeGroup)

CT_TTreeGroup::CT_TTreeGroup() : SuperClass()
{
    m_rootNode = nullptr;
}

CT_TTreeGroup::CT_TTreeGroup(const CT_TTreeGroup& other) : SuperClass(other)
{
    m_rootNode = nullptr;
}

bool CT_TTreeGroup::visitChildrensForTreeView(const CT_AbstractItem::ChildrensVisitor& visitor) const
{
    QMutexLocker lockerI(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));
    QMutexLocker lockerG(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    if(m_rootNode != nullptr)
        return visitor(m_rootNode);

    const auto visitorI = [&visitor](const CT_AbstractSingularItemDrawable* item) -> bool {

        if(!visitor(item))
            return false;

        return true;
    };

    return visitSingularItems(visitorI);
}

bool CT_TTreeGroup::setRootNodeWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* root)
{
    if(m_rootNode != nullptr)
    {
        if(!removeNode(m_rootNode))
            return false;

        m_rootNode = nullptr;
    }

    if(!addNode(outModel, root))
        return false;

    m_rootNode = root;
    m_rootNode->setTopologyTree(this);

    return true;
}

bool CT_TTreeGroup::addSuccessorWithOutModel(CT_TNodeGroup* current, const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* successor)
{
    return current->setSuccessorWithOutModel(outModel, successor);
}

bool CT_TTreeGroup::addComponentWithOutModel(CT_TNodeGroup* complex, const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* component)
{
    return complex->addComponentWithOutModel(outModel, component);
}

bool CT_TTreeGroup::addBranchWithOutModel(CT_TNodeGroup* bearer, const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* son)
{
    return bearer->addBranchWithOutModel(outModel, son);
}

CT_TNodeGroup* CT_TTreeGroup::rootNode() const
{
    return m_rootNode;
}

CT_AbstractItemDrawable* CT_TTreeGroup::copy(const CT_OutAbstractItemModel* model, const CT_AbstractResult* result) const
{
    const CT_OutAbstractGroupModel *newModel = dynamic_cast< const CT_OutAbstractGroupModel* >(model);

    CT_TTreeGroup* itemGroup = new CT_TTreeGroup(*this);
    itemGroup->setModel(model);
    itemGroup->setResult(result);

    if(newModel == nullptr)
        return itemGroup;

    // create the hash map of new models [old model, new model]
    if(m_rootNode != nullptr)
    {
        QHash<int, CT_OutAbstractGroupModel*> modelsMap;

        const auto visitor = [&modelsMap](const CT_OutAbstractModel* mm) -> bool {
            if(dynamic_cast<CT_TNodeGroup*>(mm->prototype()) != nullptr)
                modelsMap.insert(mm->uniqueIndex(), (CT_OutAbstractGroupModel*)mm);

            return true;
        };

        newModel->visitOutChildrens(visitor);

        // we must copy recursively all nodes and set it the new model
        CT_TNodeGroup *root = recursiveCopyNodes(newModel, result, itemGroup, m_rootNode, modelsMap);

        if((root == nullptr) || !itemGroup->setRootNodeWithOutModel((DEF_CT_AbstractGroupModelOut*)root->model(), root))
        {
            delete itemGroup;
            itemGroup = nullptr;
        }
    }

    return itemGroup;
}

CT_TNodeGroup* CT_TTreeGroup::recursiveCopyNodes(const CT_OutAbstractGroupModel *newModel,
                                                 const CT_AbstractResult *result,
                                                 CT_TTreeGroup *group,
                                                 CT_TNodeGroup *node,
                                                 const QHash<int, CT_OutAbstractGroupModel*> &modelsMap) const
{
    CT_OutAbstractGroupModel *newNodeModel;

    if((newNodeModel = modelsMap.value(node->model()->uniqueIndex(), nullptr)) == nullptr)
        return nullptr;

    CT_TNodeGroup *newNode = (CT_TNodeGroup*)node->copy(newNodeModel, result);
    newNode->setTopologyTree(group);

    CT_TNodeGroup *successor = node->successor();

    if(successor != nullptr)
    {
        successor = recursiveCopyNodes(newModel, result, group, successor, modelsMap);

        if((successor == nullptr) || (!newNode->setSuccessorWithOutModel((DEF_CT_AbstractGroupModelOut*)successor->model(), successor)))
        {
            delete newNode;
            delete successor;
            return nullptr;
        }
    }

    CT_TNodeGroup *rootComponent = node->rootComponent();

    if(rootComponent != nullptr)
    {
        rootComponent = recursiveCopyNodes(newModel, result, group, rootComponent, modelsMap);

        if((rootComponent == nullptr) || (!newNode->addComponentWithOutModel((DEF_CT_AbstractGroupModelOut*)rootComponent->model(), rootComponent)))
        {
            delete newNode;
            delete rootComponent;
            return nullptr;
        }
    }

    QListIterator<CT_TNodeGroup*> it(node->branches());

    while(it.hasNext())
    {
        CT_TNodeGroup *branch = it.next();

        branch = recursiveCopyNodes(newModel, result, group, branch, modelsMap);

        if((branch == nullptr) || (!newNode->addBranchWithOutModel((DEF_CT_AbstractGroupModelOut*)branch->model(), branch)))
        {
            delete newNode;
            delete branch;
            return nullptr;
        }
    }

    return newNode;
}

bool CT_TTreeGroup::addNode(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* n)
{
    addGroupWithOutModel(outModel, n);
    return true;
}

bool CT_TTreeGroup::removeNode(CT_TNodeGroup *n)
{
    return removeGroup(n);
}

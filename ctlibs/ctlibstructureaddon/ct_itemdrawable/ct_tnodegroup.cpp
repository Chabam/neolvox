#include "ct_tnodegroup.h"

#include "ct_ttreegroup.h"

CT_TYPE_IMPL_INIT_MACRO(CT_TNodeGroup)

CT_TNodeGroup::CT_TNodeGroup() : SuperClass()
{
    m_tree = nullptr;

    m_successor = nullptr;
    m_ancestor = nullptr;

    m_complex = nullptr;
    m_rootComponent = nullptr;
    m_lastComponent = nullptr;

    m_bearer = nullptr;
}

CT_TNodeGroup::CT_TNodeGroup(const CT_TNodeGroup& other) : SuperClass(other)
{
    m_tree = nullptr;

    m_successor = nullptr;
    m_ancestor = nullptr;

    m_complex = nullptr;
    m_rootComponent = nullptr;
    m_lastComponent = nullptr;

    m_bearer = nullptr;
}

bool CT_TNodeGroup::visitChildrensForTreeView(const CT_AbstractItem::ChildrensVisitor& visitor) const
{
    QMutexLocker lockerI(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));
    QMutexLocker lockerG(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    CT_TNodeGroup *o = m_rootComponent;

    if(o != nullptr)
    {
        CT_TNodeGroup* next;

        if(!visitor(o))
            return false;

        while((next = o->successor()) != nullptr) {
            if(!visitor((o = next)))
                return false;
        }
    }

    QListIterator<CT_TNodeGroup*> it(m_branches);

    while(it.hasNext()) {
        if(!visitor(it.next()))
            return false;
    }

    const auto visitorI = [&visitor](const CT_AbstractSingularItemDrawable* item) -> bool {

        if(!visitor(item))
            return false;

        return true;
    };

    return visitSingularItems(visitorI);
}

bool CT_TNodeGroup::setSuccessorWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup* successor)
{
    if(m_successor != nullptr)
    {
        if(!recursiveRemoveNode(m_successor))
            return false;

        m_successor = nullptr;
    }

    if(successor == nullptr)
        return true;

    if(!addNode(outModel, successor))
        return false;

    m_successor = successor;
    successor->setAncestor(this);
    successor->setComplex(nullptr);

    return true;
}

bool CT_TNodeGroup::addComponentWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup *component)
{
    if(m_rootComponent == nullptr)
    {
        if(!addNode(outModel, component))
            return false;

        m_rootComponent = component;
        m_lastComponent = component;
        component->setComplex(this);

        return true;
    }

    bool ok = m_lastComponent->setSuccessorWithOutModel(outModel, component);

    if(ok)
        m_lastComponent = component;

    return ok;
}

bool CT_TNodeGroup::addBranchWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup *son)
{
    if(!addNode(outModel, son))
        return false;

    m_branches.append(son);
    son->setBearer(this);
    son->setComplex(nullptr);
    son->setAncestor(nullptr);

    return true;
}

bool CT_TNodeGroup::removeComponent(CT_TNodeGroup *component, bool recursively)
{
    CT_TNodeGroup* complex = component->complex();

    component->setComplex(nullptr);

    if((component->ancestor() != nullptr) && recursively) {
        if(m_lastComponent == component)
            m_lastComponent = component->ancestor();

        return component->ancestor()->setSuccessorWithOutModel(nullptr, nullptr);
    }

    CT_TNodeGroup *suc = component->successor();
    CT_TNodeGroup *anc = component->ancestor();

    if(!recursively)
        component->internalSetSuccessor(nullptr);
    else
        suc = nullptr;

    if(!recursiveRemoveNode(component))
    {
        if(!recursively)
            component->internalSetSuccessor(suc);

        return false;
    }

    if(m_lastComponent == component)
        m_lastComponent = anc;

    if(anc != nullptr)
        anc->internalSetSuccessor(suc);
    else {
        Q_ASSERT(component == m_rootComponent);
        if (component != m_rootComponent) {qDebug() << "CT_TNodeGroup::removeComponent" << ", " << "component != m_rootComponent";}

        m_rootComponent = suc;
        m_lastComponent = m_rootComponent;

        if(suc != nullptr)
            suc->setComplex(complex);
    }

    if(suc != nullptr)
        suc->setAncestor(anc);


    return true;
}

bool CT_TNodeGroup::removeBranch(CT_TNodeGroup *son)
{
    if(m_branches.contains(son))
    {
        if(!recursiveRemoveNode(son))
            return false;

        return m_branches.removeOne(son);
    }

    return false;
}

CT_TNodeGroup* CT_TNodeGroup::ancestor() const
{
    return m_ancestor;
}

CT_TNodeGroup* CT_TNodeGroup::successor() const
{
    return m_successor;
}

CT_TNodeGroup* CT_TNodeGroup::complex() const
{
    return m_complex;
}

CT_TNodeGroup* CT_TNodeGroup::rootComponent() const
{
    return m_rootComponent;
}

CT_TNodeGroup* CT_TNodeGroup::bearer() const
{
    return m_bearer;
}

const QList<CT_TNodeGroup *>& CT_TNodeGroup::branches() const
{
    return m_branches;
}

size_t CT_TNodeGroup::nComponent() const
{
    size_t n = 0;

    CT_TNodeGroup *o = m_rootComponent;

    if(o != nullptr)
    {
        ++n;

        while(o->successor() != nullptr)
        {
            o = o->successor();
            ++n;
        }
    }

    return n;
}

CT_TTreeGroup* CT_TNodeGroup::tree() const
{
    return m_tree;
}

CT_OutAbstractModel*CT_TNodeGroup::treeModel() const
{
    return m_tree->model();
}

void CT_TNodeGroup::internalSetSuccessor(CT_TNodeGroup *successor)
{
    m_successor = successor;
}

void CT_TNodeGroup::setAncestor(CT_TNodeGroup *o)
{
    m_ancestor = o;
}

void CT_TNodeGroup::setComplex(CT_TNodeGroup *o)
{
    m_complex = o;
}

void CT_TNodeGroup::setBearer(CT_TNodeGroup *o)
{
    m_bearer = o;
}

bool CT_TNodeGroup::addNode(const DEF_CT_AbstractGroupModelOut* outModel, CT_TNodeGroup *n)
{
    if(tree() == nullptr)
        return false;

    if(n->tree() == nullptr)
        n->setTopologyTree(tree());

    return tree()->addNode(outModel, n);
}

bool CT_TNodeGroup::recursiveRemoveNode(CT_TNodeGroup *n)
{
    if(tree() == nullptr)
        return false;

    CT_TNodeGroup *s = n->rootComponent();

    if(s != nullptr)
    {
        if(!recursiveRemoveNode(s))
            return false;
    }

    s = n->successor();

    if(s != nullptr)
    {
        if(!recursiveRemoveNode(s))
            return false;
    }

    QListIterator<CT_TNodeGroup*> it(n->branches());

    while(it.hasNext())
    {
        if(!recursiveRemoveNode(it.next()))
            return false;
    }

    return tree()->removeNode(n);
}

void CT_TNodeGroup::setTopologyTree(CT_TTreeGroup *tree)
{
    m_tree = tree;
}


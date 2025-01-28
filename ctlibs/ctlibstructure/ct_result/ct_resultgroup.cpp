#include "ct_resultgroup.h"

#include "ct_model/outModel/abstract/ct_outabstractresultmodelgroup.h"

CT_ResultGroup::CT_ResultGroup() : SuperClass()
{
    m_rootGroups = new GroupContainerType(m_lockAccessTool);
}

CT_ResultGroup::CT_ResultGroup(const CT_ResultGroup& other) : SuperClass(other)
{
    m_rootGroups = new GroupContainerType(m_lockAccessTool);
}

CT_ResultGroup::~CT_ResultGroup()
{
    clearRootGroups();
}

QString CT_ResultGroup::type() const
{
    return staticType();
}

QString CT_ResultGroup::staticType()
{
    return SuperClass::staticType() + "/CT_ResultGroup";
}

bool CT_ResultGroup::isEmpty() const
{
    return (m_rootGroups->nToKeep() == 0);
}

int CT_ResultGroup::nRootGroups() const
{
    return m_rootGroups->nToKeep();
}

int CT_ResultGroup::nRootGroupsToBeRemovedLater() const
{
    return m_rootGroups->nToBeRemovedLater();
}

void CT_ResultGroup::addRootGroupWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, CT_StandardItemGroup* rootGroup)
{
    Q_ASSERT(rootGroup != nullptr);
    if (rootGroup == nullptr) {qDebug() << "CT_ResultGroup::addRootGroupWithOutModel" << ", " << "rootGroup == nullptr";}

    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_ResultGroup::addRootGroupWithOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(outModel->parentModel() == model());
    if (outModel->parentModel() != model()) {qDebug() << "CT_ResultGroup::addRootGroupWithOutModel" << ", " << "outModel->parentModel() != model()";}

    QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

    m_rootGroups->append(rootGroup);

    rootGroup->setParentContainer(m_rootGroups);
    rootGroup->setModel(outModel);
    rootGroup->setResult(this);
}

void CT_ResultGroup::addRootGroupsWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel, const QList<CT_StandardItemGroup*>& rootGroups)
{
    if(rootGroups.isEmpty())
        return;

    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_ResultGroup::addRootGroupsWithOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(outModel->parentModel() == model());
    if (outModel->parentModel() != model()) {qDebug() << "CT_ResultGroup::addRootGroupsWithOutModel" << ", " << "outModel->parentModel() != model()";}

    QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

    m_rootGroups->append(rootGroups);

    for(CT_StandardItemGroup* rootGroup : rootGroups) {
        rootGroup->setParentContainer(m_rootGroups);
        rootGroup->setModel(outModel);
        rootGroup->setResult(this);
    }
}

CT_ResultGroup::iterator CT_ResultGroup::begin()
{
    return m_rootGroups->begin();
}

CT_ResultGroup::iterator CT_ResultGroup::end()
{
    return m_rootGroups->end();
}

CT_ResultGroup::const_iterator CT_ResultGroup::constBegin() const
{
    return m_rootGroups->constBegin();
}

CT_ResultGroup::const_iterator CT_ResultGroup::constEnd() const
{
    return m_rootGroups->constEnd();
}

bool CT_ResultGroup::visitChildrens(const CT_AbstractItem::ChildrensVisitor& visitor) const
{
    return m_rootGroups->visitElementsToKeep([&visitor](const CT_StandardItemGroup* rootGroup) -> bool {
        return visitor(rootGroup);
    });
}

int CT_ResultGroup::nChildrens() const
{
    return m_rootGroups->nToKeep();
}

CT_AbstractItem::ChildrensCollection CT_ResultGroup::childrensThatUseOutModel(const CT_OutAbstractModel* outModel) const
{
    Q_ASSERT(model() != nullptr);
    if (model() == nullptr) {qDebug() << "CT_ResultGroup::childrensThatUseOutModel" << ", " << "model() == nullptr";}

    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_ResultGroup::childrensThatUseOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(outModel == static_cast<CT_OutAbstractResultModelGroup*>(model())->rootGroup());
    if (outModel != static_cast<CT_OutAbstractResultModelGroup*>(model())->rootGroup()) {qDebug() << "CT_ResultGroup::childrensThatUseOutModel" << ", " << "outModel != static_cast<CT_OutAbstractResultModelGroup*>(model())->rootGroup()";}

    return CT_AbstractItem::ChildrensCollection(CT_AbstractItem::IChildrensIteratorQtStylePtr(new GroupIterator(m_rootGroups->begin(), m_rootGroups->end())));
}

CT_AbstractResult* CT_ResultGroup::copy(const CT_OutAbstractResultModel* newModelToUse) const
{
    Q_ASSERT(dynamic_cast<const CT_OutAbstractResultModelGroup*>(newModelToUse) != nullptr);
    if (dynamic_cast<const CT_OutAbstractResultModelGroup*>(newModelToUse) == nullptr) {qDebug() << "CT_ResultGroup::copy" << ", " << "dynamic_cast<const CT_OutAbstractResultModelGroup*>(newModelToUse) == nullptr";}

    CT_ResultGroup* c = new CT_ResultGroup(*this);
    c->setModel(newModelToUse);

    const DEF_CT_AbstractGroupModelOut* rootGroupModel = static_cast<const CT_OutAbstractResultModelGroup*>(newModelToUse)->rootGroup();

    if(rootGroupModel != nullptr) {

        const auto visitor = [&c, &rootGroupModel](const CT_StandardItemGroup* rootGroup) -> bool {
            CT_StandardItemGroup* rootGroupCpy = static_cast<CT_StandardItemGroup*>(rootGroup->copy(rootGroupModel, c));
            c->m_rootGroups->append(rootGroupCpy);
            rootGroupCpy->setParentContainer(c->m_rootGroups);

            return true;
        };

        m_rootGroups->visitElementsToKeep(visitor);
    }

    return c;
}

IPrototypeToolForModel* CT_ResultGroup::prototypeToolForModel() const
{
    return SuperClass::prototypeToolForModel();
}

IResultToolForModel* CT_ResultGroup::resultToolForModel() const
{
    return SuperClass::resultToolForModel();
}

void CT_ResultGroup::setComplete()
{
    QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

    m_rootGroups->clearElementsToBeRemovedLater();

    const auto visitor = [](const CT_StandardItemGroup* rootGroup) -> bool {
        const_cast<CT_StandardItemGroup*>(rootGroup)->recursiveSetComplete();
        return true;
    };

    m_rootGroups->visitElementsToKeep(visitor);

    CT_AbstractResult::setComplete();
}

void CT_ResultGroup::internalClearFromMemory()
{
    clearRootGroups();
}

CT_AbstractItem::IChildrensIteratorQtStylePtr CT_ResultGroup::createQtStyleIteratorForChildrensThatUseOutModel(const CT_OutAbstractModel*) const
{
    return IChildrensIteratorQtStylePtr(new GroupIterator(m_rootGroups->begin(), m_rootGroups->end()));
}

void CT_ResultGroup::clearRootGroups()
{
    QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

    PS_STRUCTURECONTEXT->informThatWeStartToDeleteMultipleCloud();

    const int size = m_rootGroups->nInTotal();

    QVector<CT_StandardItemGroup*> groups(size, nullptr);
    int i = 0;

    const auto visitor = [this, &groups, &i](const CT_StandardItemGroup* group) -> bool {
        CT_StandardItemGroup* gr = const_cast<CT_StandardItemGroup*>(group);
        gr->willBeRemovedFromResult(this);
        groups[i++] = gr;

        return true;
    };

    m_rootGroups->visitAllElements(visitor);

    for(int j=0; j<i; ++j) {
        m_rootGroups->remove(groups[j]);
        setClearFromMemoryProgress((j*100)/i);
    }

    PS_STRUCTURECONTEXT->informThatWeHaveFinishedToDeleteMultipleCloud();
}

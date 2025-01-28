#include "ct_standarditemgroup.h"

#include "ct_model/inModel/abstract/ct_inabstractsingularitemmodel.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"

const CT_StandardStandardItemGroupDrawManager CT_StandardItemGroup::SIG_DRAW_MANAGER;

CT_TYPE_IMPL_INIT_MACRO(CT_StandardItemGroup)

CT_StandardItemGroup::CT_StandardItemGroup() : SuperClass(),
    m_removedLaterFlags(Nothing),
    m_parentContainer(nullptr),
    m_context(nullptr)
{
    setBaseDrawManager(&SIG_DRAW_MANAGER);
}

CT_StandardItemGroup::CT_StandardItemGroup(const CT_StandardItemGroup& other) : SuperClass(other),
    m_removedLaterFlags(other.m_removedLaterFlags),
    m_parentContainer(nullptr),
    m_context(other.m_context)
{
}

CT_StandardItemGroup::~CT_StandardItemGroup()
{
    qDeleteAll(m_newGroupsAdded.begin(), m_newGroupsAdded.end());
    qDeleteAll(m_groupsCopied.begin(), m_groupsCopied.end());

    qDeleteAll(m_newItemsAdded.begin(), m_newItemsAdded.end());
    qDeleteAll(m_itemsCopied.begin(), m_itemsCopied.end());
}

bool CT_StandardItemGroup::visitChildrensForTreeView(const CT_AbstractItem::ChildrensVisitor& visitor) const
{
    QMutexLocker lockerI(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));
    QMutexLocker lockerG(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    const auto visitorG = [&visitor](const CT_StandardItemGroup* group) -> bool {

        if(!visitor(group))
            return false;

        return true;
    };

    if(!visitGroups(visitorG))
        return false;

    const auto visitorI = [&visitor](const CT_AbstractSingularItemDrawable* item) -> bool {

        if(!visitor(item))
            return false;

        return true;
    };

    return visitSingularItems(visitorI);
}

bool CT_StandardItemGroup::visitChildrens(const CT_AbstractItem::ChildrensVisitor& visitor) const
{
    QMutexLocker lockerI(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));
    QMutexLocker lockerG(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    const auto visitorG = [&visitor](const CT_StandardItemGroup* group) -> bool {

        if(!visitor(group))
            return false;

        return true;
    };

    if(!visitGroups(visitorG))
        return false;

    const auto visitorI = [&visitor](const CT_AbstractSingularItemDrawable* item) -> bool {

        if(!visitor(item))
            return false;

        return true;
    };

    return visitSingularItems(visitorI);
}

bool CT_StandardItemGroup::isEmpty() const
{
    QMutexLocker lockerI(m_lockAccessTool.m_mutexAccessItem);
    QMutexLocker lockerG(m_lockAccessTool.m_mutexAccessGroup);

    return ((nGroup() == 0) && (nSingularItem() == 0));
}

double CT_StandardItemGroup::centerX() const
{
    QMutexLocker lockerI(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));
    QMutexLocker lockerG(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    double center = 0;
    int size = 0;

    const auto visitorG = [&center, &size](const CT_StandardItemGroup* group) -> bool {
        center += group->centerX();
        ++size;
        return true;
    };

    visitGroups(visitorG);

    const auto visitorI = [&center, &size](const CT_AbstractSingularItemDrawable* item) -> bool {
        center += item->centerX();
        ++size;
        return true;
    };

    visitSingularItems(visitorI);

    if(size != 0)
        return center/double(size);

    return 0.0;
}

double CT_StandardItemGroup::centerY() const
{
    QMutexLocker lockerI(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));
    QMutexLocker lockerG(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    double center = 0;
    int size = 0;

    const auto visitorG = [&center, &size](const CT_StandardItemGroup* group) -> bool {
        center += group->centerY();
        ++size;
        return true;
    };

    visitGroups(visitorG);

    const auto visitorI = [&center, &size](const CT_AbstractSingularItemDrawable* item) -> bool {
        center += item->centerY();
        ++size;
        return true;
    };

    visitSingularItems(visitorI);

    if(size != 0)
        return center/double(size);

    return 0.0;
}

double CT_StandardItemGroup::centerZ() const
{
    QMutexLocker lockerI(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));
    QMutexLocker lockerG(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    double center = 0;
    int size = 0;

    const auto visitorG = [&center, &size](const CT_StandardItemGroup* group) -> bool {
        center += group->centerZ();
        ++size;
        return true;
    };

    visitGroups(visitorG);

    const auto visitorI = [&center, &size](const CT_AbstractSingularItemDrawable* item) -> bool {
        center += item->centerZ();
        ++size;
        return true;
    };

    visitSingularItems(visitorI);

    if(size != 0)
        return center/double(size);

    return 0.0;
}

int CT_StandardItemGroup::nChildrens() const
{
    QMutexLocker lockerI(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));
    QMutexLocker lockerG(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    return nGroup() + nSingularItem();
}

bool CT_StandardItemGroup::visitChildrensOfTypeItem(const CT_AbstractItemDrawable::ItemVisitor& visitor) const
{
    QMutexLocker lockerI(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));
    QMutexLocker lockerG(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    const auto visitorGroupAdapter = [&visitor](const CT_StandardItemGroup* child) -> bool {
        return visitor(child);
    };

    if(!visitGroups(visitorGroupAdapter))
        return false;

    const auto visitorItemAdapter = [&visitor](const CT_AbstractSingularItemDrawable* child) -> bool {
        return visitor(child);
    };

    return visitSingularItems(visitorItemAdapter);
}

IPrototypeToolForModel* CT_StandardItemGroup::prototypeToolForModel() const
{
    return SuperClass::prototypeToolForModel();
}

IItemDrawableToolForModel* CT_StandardItemGroup::itemToolForModel() const
{
    return SuperClass::itemToolForModel();
}

void CT_StandardItemGroup::addSingularItemWithOutModel(const DEF_CT_AbstractItemDrawableModelOut* outModel, CT_AbstractSingularItemDrawable* item)
{
    Q_ASSERT(item != nullptr);
    if (item == nullptr) {qDebug() << "CT_StandardItemGroup::addSingularItemWithOutModel" << ", " << "item == nullptr";}

    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::addSingularItemWithOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(outModel->parentModel() == model());
    if (outModel->parentModel() != model()) {qDebug() << "CT_StandardItemGroup::addSingularItemWithOutModel" << ", " << "outModel->parentModel() != model()";}

    QMutexLocker locker(m_lockAccessTool.m_mutexAccessItem);

    const OutModelKeyType key = CT_StandardItemGroup::outModelKey(outModel);

    ItemContainerType* container = m_newItemsAdded.value(key, nullptr);

    // if the container already exist it means that this item was
    // already be added to this group
    if(container != nullptr) {
        // so we undo the "remove later" flag
        container->undoWillBeRemovedLater();
        return;
    }

    item->setModel(outModel);
    item->setParentItem(this);
    item->setResult(static_cast<CT_AbstractResult*>(outModel->result()));

    m_newItemsAdded.insert(key, new ItemContainerType(item));
}

bool CT_StandardItemGroup::removeSingularItemWithOutModel(const DEF_CT_AbstractItemDrawableModelOut* outModel)
{
    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::removeSingularItemWithOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() == model()->uniqueIndex());
    if (static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() != model()->uniqueIndex()) {qDebug() << "CT_StandardItemGroup::removeSingularItemWithOutModel" << ", " << "static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() != model()->uniqueIndex()";}

    QMutexLocker locker(m_lockAccessTool.m_mutexAccessItem);

    ItemContainerType* ct = itemContainerWithOutModel(outModel);

    if(ct != nullptr)
    {
        ct->setWillBeRemovedLater();
        return true;
    }

    return false;
}

bool CT_StandardItemGroup::removeSingularItem(const CT_AbstractSingularItemDrawable* item)
{
    Q_ASSERT(item != nullptr);
    if (item == nullptr) {qDebug() << "CT_StandardItemGroup::removeSingularItem" << ", " << "item == nullptr";}

    QMutexLocker locker(m_lockAccessTool.m_mutexAccessItem);

    ItemContainerType* ct = itemContainerWithOutModel(item->model());

    if(ct != nullptr && ct->item() == item)
    {
        ct->setWillBeRemovedLater();
        return true;
    }

    return false;
}

bool CT_StandardItemGroup::containsSingularItemInSelectedPossibilitiesOfInModel(const DEF_CT_AbstractItemDrawableModelIn* inModel) const
{
    Q_ASSERT(inModel != nullptr);
    if (inModel == nullptr) {qDebug() << "CT_StandardItemGroup::containsSingularItemInSelectedPossibilitiesOfInModel" << ", " << "inModel == nullptr";}

    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    const auto visitor = [this](const CT_InStdModelPossibility* possibility) -> bool {

        if(this->containsSingularItemWithOutModel(static_cast<DEF_CT_AbstractItemDrawableModelOut*>(possibility->outModel())))
            return false;

        return true;
    };

    return !inModel->possibilitiesGroup()->visitSelectedPossibilities(visitor);
}

bool CT_StandardItemGroup::containsSingularItemWithOutModel(const DEF_CT_AbstractItemDrawableModelOut* outModel) const
{
    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::containsSingularItemWithOutModel" << ", " << "outModel == nullptr";}

    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    return singularItemWithOutModel(outModel) != nullptr;
}

bool CT_StandardItemGroup::containsSingularItem(const CT_AbstractSingularItemDrawable* item) const
{
    Q_ASSERT(item != nullptr);
    if (item == nullptr) {qDebug() << "CT_StandardItemGroup::containsSingularItem" << ", " << "item == nullptr";}

    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    return item == internalSingularItemWithOutModel(item->model());
}

int CT_StandardItemGroup::nSingularItem() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    int n = 0;

    ItemContainerCollectionIterator it(m_newItemsAdded);

    while(it.hasNext())
    {
        if(!it.next().value()->willBeRemovedLater())
            ++n;
    }

    it = m_itemsCopied;

    while(it.hasNext())
    {
        if(!it.next().value()->willBeRemovedLater())
            ++n;
    }

    return n;
}

int CT_StandardItemGroup::nSingularItemToBeRemovedLater() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    int n = 0;

    ItemContainerCollectionIterator it(m_newItemsAdded);

    while(it.hasNext())
    {
        if(it.next().value()->willBeRemovedLater())
            ++n;
    }

    it = m_itemsCopied;

    while(it.hasNext())
    {
        if(it.next().value()->willBeRemovedLater())
            ++n;
    }

    return n;
}

int CT_StandardItemGroup::nSingularItemInTotal() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    return m_newItemsAdded.size() + m_itemsCopied.size();
}

bool CT_StandardItemGroup::visitSingularItems(const CT_StandardItemGroup::SingularItemVisitor& visitor) const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    if(!visitNewSingularItems(visitor))
        return false;

    return visitBackupSingularItems(visitor);
}

CT_StandardItemGroup::SingularItemIterator CT_StandardItemGroup::singularItems() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    using iterator = ItemContainerType::const_iterator;
    using info = QPair<iterator, iterator>;

    QVector<info> infos;
    infos.resize(m_newItemsAdded.size() + m_itemsCopied.size());
    int currentIndex = 0;

    ItemContainerCollectionIterator it(m_newItemsAdded);

    while(it.hasNext()) {
        ItemContainerType* container = it.next().value();
        infos[currentIndex++] = qMakePair(container->constBegin(), container->constEnd());
    }

    it = m_itemsCopied;

    while(it.hasNext()) {
        ItemContainerType* container = it.next().value();
        infos[currentIndex++] = qMakePair(container->constBegin(), container->constEnd());
    }

    return SingularItemIterator(MultiItemCollectionIterator::create(infos), MultiItemCollectionIterator());
}

bool CT_StandardItemGroup::visitNewSingularItems(const CT_StandardItemGroup::SingularItemVisitor& visitor) const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    ItemContainerCollectionIterator it(m_newItemsAdded);

    while(it.hasNext())
    {
        const ItemContainerType* container = it.next().value();
        if(!container->willBeRemovedLater()) {
            if(!visitor(container->item()))
                return false;
        }
    }

    return true;
}

CT_StandardItemGroup::SingularItemIterator CT_StandardItemGroup::newSingularItems() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    using iterator = ItemContainerType::const_iterator;
    using info = QPair<iterator, iterator>;

    QVector<info> infos;
    infos.resize(m_newItemsAdded.size());
    int currentIndex = 0;

    ItemContainerCollectionIterator it(m_newItemsAdded);

    while(it.hasNext()) {
        ItemContainerType* container = it.next().value();
        infos[currentIndex++] = qMakePair(container->constBegin(), container->constEnd());
    }

    return SingularItemIterator(MultiItemCollectionIterator::create(infos), MultiItemCollectionIterator());
}

bool CT_StandardItemGroup::visitBackupSingularItems(const CT_StandardItemGroup::SingularItemVisitor& visitor) const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    ItemContainerCollectionIterator it(m_itemsCopied);

    while(it.hasNext())
    {
        const ItemContainerType* container = it.next().value();
        if(!container->willBeRemovedLater()) {
            if(!visitor(container->item()))
                return false;
        }
    }

    return true;
}

CT_StandardItemGroup::SingularItemIterator CT_StandardItemGroup::backupSingularItems() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    using iterator = ItemContainerType::const_iterator;
    using info = QPair<iterator, iterator>;

    QVector<info> infos;
    infos.resize(m_itemsCopied.size());
    int currentIndex = 0;

    ItemContainerCollectionIterator it(m_itemsCopied);

    while(it.hasNext()) {
        ItemContainerType* container = it.next().value();
        infos[currentIndex++] = qMakePair(container->constBegin(), container->constEnd());
    }

    return SingularItemIterator(MultiItemCollectionIterator::create(infos), MultiItemCollectionIterator());
}

CT_AbstractSingularItemDrawable* CT_StandardItemGroup::singularItemWithOutModel(const DEF_CT_AbstractItemDrawableModelOut* outModel) const
{
    return internalSingularItemWithOutModel(outModel);
}

bool CT_StandardItemGroup::visitSingularItemsInSelectedPossibilitiesOfInModel(const DEF_CT_AbstractItemDrawableModelIn* inModel,
                                                                              const CT_StandardItemGroup::SingularItemVisitor& visitor) const
{
    Q_ASSERT(inModel != nullptr);
    if (inModel == nullptr) {qDebug() << "CT_StandardItemGroup::visitSingularItemsInSelectedPossibilitiesOfInModel" << ", " << "inModel == nullptr";}

    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    const auto visitorAdapter = [this, &visitor](const CT_InStdModelPossibility* possibility) -> bool {

        const CT_AbstractSingularItemDrawable* item = this->internalSingularItemWithOutModel(possibility->outModel());

        if(item != nullptr) {
            if(!visitor(item))
                return false;
        }

        return true;
    };

    return !inModel->possibilitiesGroup()->visitSelectedPossibilities(visitorAdapter);
}

void CT_StandardItemGroup::addGroupWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel,
                                                CT_StandardItemGroup* group)
{
    Q_ASSERT(group != nullptr);
    if (group == nullptr) {qDebug() << "CT_StandardItemGroup::addGroupWithOutModel" << ", " << "group == nullptr";}

    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::addGroupWithOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(outModel->parentModel() == model());
    if (outModel->parentModel() != model()) {qDebug() << "CT_StandardItemGroup::addGroupWithOutModel" << ", " << "outModel->parentModel() != model()";}

    QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

    const OutModelKeyType key = CT_StandardItemGroup::outModelKey(outModel);

    GroupContainerType* container = m_newGroupsAdded.value(key, nullptr);

    if(container == nullptr) {
        container = new GroupContainerType(m_lockAccessTool);
        m_newGroupsAdded.insert(key, container);
    } else {
        Q_ASSERT(group->willBeRemovedLater() || !container->contains(group));
        if (!group->willBeRemovedLater() && container->contains(group)) {qDebug() << "CT_StandardItemGroup::addGroupWithOutModel" << ", " << "!group->willBeRemovedLater() && container->contains(group)";}
    }

    if(!group->willBeRemovedLater()) {
        group->setModel(outModel);
        group->setResult(static_cast<CT_AbstractResult*>(outModel->result()));
        group->setParentGroup(this);
        group->setParentContainer(container);
        container->append(group);
        return;
    }

    // if it has already be added to this group
    // we undo the "remove later" flag
    group->undoWillBeRemovedLater();
}

bool CT_StandardItemGroup::removeGroupsWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel)
{
    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::removeGroupsWithOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() == model()->uniqueIndex());
    if (static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() != model()->uniqueIndex()) {qDebug() << "CT_StandardItemGroup::removeGroupsWithOutModel" << ", " << "static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() != model()->uniqueIndex()";}

    QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

    GroupContainerType* container = groupContainerWithOutModel(outModel);

    if(container == nullptr)
        return false;

    static const auto visitor = [](const CT_StandardItemGroup* group) -> bool {
        const_cast<CT_StandardItemGroup*>(group)->setBeRemovedLater();
        return true;
    };

    container->visitElementsToKeep(visitor);

    return true;
}

bool CT_StandardItemGroup::removeGroup(CT_StandardItemGroup* group)
{
    Q_ASSERT(group != nullptr);
    if (group == nullptr) {qDebug() << "CT_StandardItemGroup::removeGroup" << ", " << "group == nullptr";}

    Q_ASSERT(group->model() != nullptr);
    if (group->model() == nullptr) {qDebug() << "CT_StandardItemGroup::removeGroup" << ", " << "group->model() == nullptr";}

    Q_ASSERT(group->parentGroup() == this);
    if (group->parentGroup() != this) {qDebug() << "CT_StandardItemGroup::removeGroup" << ", " << "group->parentGroup() != this";}

    QMutexLocker locker(m_lockAccessTool.m_mutexAccessGroup);

    group->setBeRemovedLater();

    return true;
}

void CT_StandardItemGroup::removeFromParent(bool ifEmptyRemoveParentTooRecursively)
{
    setBeRemovedLater(ifEmptyRemoveParentTooRecursively);
}

bool CT_StandardItemGroup::visitGroups(const CT_StandardItemGroup::GroupVisitor& visitor) const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    if(!visitNewGroups(visitor))
        return false;

    return visitBackupGroups(visitor);
}

CT_StandardItemGroup::GroupIterator CT_StandardItemGroup::groups() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    using iterator = GroupContainerType::const_iterator;
    using info = QPair<iterator, iterator>;

    QVector<info> infos;
    infos.resize(m_newGroupsAdded.size() + m_groupsCopied.size());
    int currentIndex = 0;

    GroupContainerCollectionIterator it(m_newGroupsAdded);

    while(it.hasNext()) {
        GroupContainerType* container = it.next().value();
        infos[currentIndex++] = qMakePair(container->constBegin(), container->constEnd());
    }

    it = m_groupsCopied;

    while(it.hasNext()) {
        GroupContainerType* container = it.next().value();
        infos[currentIndex++] = qMakePair(container->constBegin(), container->constEnd());
    }

    return GroupIterator(MultiGroupCollectionIterator::create(infos), MultiGroupCollectionIterator());
}

bool CT_StandardItemGroup::visitNewGroups(const CT_StandardItemGroup::GroupVisitor& visitor) const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    const auto visitorI = [&visitor](const CT_StandardItemGroup* group) -> bool {
        return visitor(group);
    };

    GroupContainerCollectionIterator it(m_newGroupsAdded);

    while(it.hasNext())
    {
        const GroupContainerType* container = it.next().value();

        if(!container->visitElementsToKeep(visitorI))
            return false;
    }

    return true;
}

CT_StandardItemGroup::GroupIterator CT_StandardItemGroup::newGroups() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    using iterator = GroupContainerType::const_iterator;
    using info = QPair<iterator, iterator>;

    QVector<info> infos;
    infos.resize(m_newGroupsAdded.size());
    int currentIndex = 0;

    GroupContainerCollectionIterator it(m_newGroupsAdded);

    while(it.hasNext()) {
        GroupContainerType* container = it.next().value();
        infos[currentIndex++] = qMakePair(container->constBegin(), container->constEnd());
    }

    return GroupIterator(MultiGroupCollectionIterator::create(infos), MultiGroupCollectionIterator());
}

bool CT_StandardItemGroup::visitBackupGroups(const CT_StandardItemGroup::GroupVisitor& visitor) const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    const auto visitorI = [&visitor](const CT_StandardItemGroup* item) -> bool {
        return visitor(item);
    };

    GroupContainerCollectionIterator it(m_groupsCopied);

    while(it.hasNext())
    {
        const GroupContainerType* container = it.next().value();

        if(!container->visitElementsToKeep(visitorI))
            return false;
    }

    return true;
}

CT_StandardItemGroup::GroupIterator CT_StandardItemGroup::backupGroups() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    using iterator = GroupContainerType::const_iterator;
    using info = QPair<iterator, iterator>;

    QVector<info> infos;
    infos.resize(m_groupsCopied.size());
    int currentIndex = 0;

    GroupContainerCollectionIterator it(m_groupsCopied);

    while(it.hasNext()) {
        GroupContainerType* container = it.next().value();
        infos[currentIndex++] = qMakePair(container->constBegin(), container->constEnd());
    }

    return GroupIterator(MultiGroupCollectionIterator::create(infos), MultiGroupCollectionIterator());
}

int CT_StandardItemGroup::nGroup() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    int n = 0;

    GroupContainerCollectionIterator it(m_newGroupsAdded);

    while(it.hasNext())
        n += it.next().value()->nToKeep();

    it = m_groupsCopied;

    while(it.hasNext())
        n += it.next().value()->nToKeep();

    return n;
}

int CT_StandardItemGroup::nGroupToBeRemovedLater() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    int n = 0;

    GroupContainerCollectionIterator it(m_newGroupsAdded);

    while(it.hasNext())
        n += it.next().value()->nToBeRemovedLater();

    it = m_groupsCopied;

    while(it.hasNext())
        n += it.next().value()->nToBeRemovedLater();

    return n;
}

int CT_StandardItemGroup::nGroupInTotal() const
{
    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    int n = 0;

    GroupContainerCollectionIterator it(m_newGroupsAdded);

    while(it.hasNext())
        n += it.next().value()->nInTotal();

    it = m_groupsCopied;

    while(it.hasNext())
        n += it.next().value()->nInTotal();

    return n;
}

CT_StandardItemGroup* CT_StandardItemGroup::groupWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel) const
{
    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::groupWithOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() == model()->uniqueIndex());
    if (static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() != model()->uniqueIndex()) {qDebug() << "CT_StandardItemGroup::groupWithOutModel" << ", " << "static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() != model()->uniqueIndex()";}

    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    GroupContainerType* container = groupContainerWithOutModel(outModel);

    if(container == nullptr)
        return nullptr;

    CT_StandardItemGroup* group = nullptr;
    container->visitElementsToKeep([&group](const CT_StandardItemGroup* gr) -> bool {
        group = const_cast<CT_StandardItemGroup*>(gr);
        return false;
    });

    return group;
}

bool CT_StandardItemGroup::visitGroupsInSelectedPossibilitiesOfInModel(const DEF_CT_AbstractGroupModelIn* inModel, const CT_StandardItemGroup::GroupVisitor& visitor) const
{
    Q_ASSERT(inModel != nullptr);
    if (inModel == nullptr) {qDebug() << "CT_StandardItemGroup::visitGroupsInSelectedPossibilitiesOfInModel" << ", " << "inModel == nullptr";}

    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    const auto visitorAdapter = [this, &visitor](const CT_InStdModelPossibility* possibility) -> bool {

        const CT_StandardItemGroup* group = this->groupWithOutModel(static_cast<DEF_CT_AbstractGroupModelOut*>(possibility->outModel()));

        if(group != nullptr) {
            if(!visitor(group))
                return false;
        }

        return true;
    };

    return !inModel->possibilitiesGroup()->visitSelectedPossibilities(visitorAdapter);
}

bool CT_StandardItemGroup::containsGroupWithOutModel(const DEF_CT_AbstractGroupModelOut* outModel) const
{
    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::containsGroupWithOutModel" << ", " << "outModel == nullptr";}

    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

    if(static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() != model()->uniqueIndex())
        return false;

    GroupContainerType* container = groupContainerWithOutModel(outModel);

    if(container == nullptr)
        return false;

    return (container->nToKeep() != 0);
}

CT_StandardItemGroup* CT_StandardItemGroup::parentGroup() const
{
    return static_cast<CT_StandardItemGroup*>(parentItem());
}

void CT_StandardItemGroup::setParentGroup(const CT_StandardItemGroup* pGroup)
{
    setParentItem(pGroup);
}

void CT_StandardItemGroup::setParentContainer(const GroupContainerType* container)
{
    m_parentContainer = const_cast<GroupContainerType*>(container);
}

CT_StandardItemGroup::GroupContainerType* CT_StandardItemGroup::parentContainer() const
{
    return m_parentContainer;
}

void CT_StandardItemGroup::setContext(const void* context)
{
    m_context = const_cast<void*>(context);
}

void* CT_StandardItemGroup::getContext() const
{
    return m_context;
}

bool CT_StandardItemGroup::willBeRemovedLater() const
{
    return m_removedLaterFlags.testFlag(RemoveLater);
}

bool CT_StandardItemGroup::atLeastOneChildMustBeRemovedLater() const
{
    return m_removedLaterFlags.testFlag(ChildRemoveLater);
}

void CT_StandardItemGroup::setBeRemovedLater(bool ifEmptyRemoveParentTooRecursively)
{
    Q_ASSERT(parentContainer() != nullptr);
    if (parentContainer() == nullptr) {qDebug() << "CT_StandardItemGroup::setBeRemovedLater" << ", " << "parentContainer() == nullptr";}

    parentContainer()->lock();

    if(parentGroup() != nullptr)
        parentGroup()->setAtLeastOneChildMustBeRemovedLater();

    parentContainer()->setToBeRemovedLater(this);
    m_removedLaterFlags |= RemoveLater;

    parentContainer()->unlock();

    if(ifEmptyRemoveParentTooRecursively && (parentGroup() != nullptr) && (parentGroup()->isEmpty()))
        parentGroup()->removeFromParent(true);
}

void CT_StandardItemGroup::setAtLeastOneChildMustBeRemovedLater()
{
    parentContainer()->lock();

    if(atLeastOneChildMustBeRemovedLater()) {
        parentContainer()->unlock();
        return;
    }

    m_removedLaterFlags |= ChildRemoveLater;

    if(parentGroup() != nullptr)
        parentGroup()->setAtLeastOneChildMustBeRemovedLater();

    parentContainer()->unlock();
}

void CT_StandardItemGroup::undoWillBeRemovedLater()
{
    Q_ASSERT(parentContainer() != nullptr);
    if (parentContainer() == nullptr) {qDebug() << "CT_StandardItemGroup::undoWillBeRemovedLater" << ", " << "parentContainer() == nullptr";}

    parentContainer()->lock();

    m_removedLaterFlags &= ~RemoveLater;
    parentContainer()->setToKeep(this);

    parentContainer()->unlock();
}

void CT_StandardItemGroup::recursiveSetComplete()
{
    static const auto visitorSetComplete = [](const CT_StandardItemGroup* rootGroup) -> bool {
        const_cast<CT_StandardItemGroup*>(rootGroup)->recursiveSetComplete();
        return true;
    };

    QMutableHashIterator<OutModelKeyType, GroupContainerType*> itG(m_newGroupsAdded);

    while(itG.hasNext()) {
        GroupContainerType* container = itG.next().value();
        container->clearElementsToBeRemovedLater();

        if(container->nToKeep() == 0) {
            delete container;
            itG.remove();
        } else {
            container->visitElementsToKeep(visitorSetComplete);
        }
    }

    itG = m_groupsCopied;

    while(itG.hasNext()) {
        GroupContainerType* container = itG.next().value();
        container->clearElementsToBeRemovedLater();

        if(container->nToKeep() == 0) {
            delete container;
            itG.remove();
        } else {
            container->visitElementsToKeep(visitorSetComplete);
        }
    }

    QMutableHashIterator<OutModelKeyType, ItemContainerType*> itI(m_newItemsAdded);

    while(itI.hasNext()) {
        ItemContainerType* container = itI.next().value();

        if(container->willBeRemovedLater()) {
            delete container;
            itI.remove();
        }
    }

    itI = m_itemsCopied;

    while(itI.hasNext()) {
        ItemContainerType* container = itI.next().value();

        if(container->willBeRemovedLater()) {
            delete container;
            itI.remove();
        }
    }
}

void CT_StandardItemGroup::willBeRemovedFromResult(const CT_AbstractResult* res)
{
    const auto visitorG = [&res](const CT_StandardItemGroup* group) -> bool {
        const_cast<CT_StandardItemGroup*>(group)->willBeRemovedFromResult(res);
        return true;
    };

    visitGroups(visitorG);

    const auto visitorI = [&res](const CT_AbstractSingularItemDrawable* item) -> bool {
        const_cast<CT_AbstractSingularItemDrawable*>(item)->willBeRemovedFromResult(res);
        return true;
    };

    visitSingularItems(visitorI);
}

CT_AbstractItemDrawable* CT_StandardItemGroup::copy(const CT_OutAbstractItemModel* model, const CT_AbstractResult* result) const
{
    CT_StandardItemGroup* c = new CT_StandardItemGroup(*this);
    c->setModel(model);
    c->setResult(result);

    if(model != nullptr)
        copyChildrensToGroupIfTheyAreStillPresentInModel(c);

    return c;
}

void CT_StandardItemGroup::copyChildrensToGroupIfTheyAreStillPresentInModel(CT_StandardItemGroup* itemGroup) const
{
    const CT_AbstractResult* result = itemGroup->result();

    // copy backup groups to backup groups of the new group if their are still present
    staticCopyGroupsFromXToBackupIfChildrenGroupsAreStillPresent(result, *itemGroup, m_groupsCopied);

    // copy new groups to backup groups of the new group if their are still present
    staticCopyGroupsFromXToBackupIfChildrenGroupsAreStillPresent(result, *itemGroup, m_newGroupsAdded);

    // copy backup items to backup items of the new group if their are still present
    staticCopyItemsFromXToBackupIfChildrenItemsAreStillPresent(result, *itemGroup, m_itemsCopied);

    // copy new items to backup items of the new group if their are still present
    staticCopyItemsFromXToBackupIfChildrenItemsAreStillPresent(result, *itemGroup, m_newItemsAdded);
}

void CT_StandardItemGroup::staticCopyGroupsFromXToBackupIfChildrenGroupsAreStillPresent(const CT_AbstractResult* result,
                                                                                        CT_StandardItemGroup& parentGroup,
                                                                                        const GroupsCollection& mapSource)
{
    DEF_CT_AbstractGroupModelOut* parentGroupModel = static_cast<DEF_CT_AbstractGroupModelOut*>(parentGroup.model());

    // visit all childrens (groups) that are still present in the parent group. So groups
    // that was removed from the group will not be copied.
    const auto visitor = [&](const DEF_CT_AbstractGroupModelOut* childGroupModel) -> bool {

        const OutModelKeyType key = CT_StandardItemGroup::outModelCopyKey(childGroupModel);
        const OutModelKeyType newKey = CT_StandardItemGroup::outModelKey(childGroupModel);

        // if the unique index of the child group model is not present in the mapSource
        // it is because it was a new group that will be added when step compute
        GroupContainerType* containerSource = mapSource.value(key, nullptr);

        // if child group model is still present
        if(containerSource != nullptr) {

            // we copy all groups and add it to a new container
            GroupContainerType* containerCopy = new GroupContainerType(parentGroup.m_lockAccessTool);

            for(const CT_StandardItemGroup* groupToCopy : *containerSource)
            {
                CT_StandardItemGroup* groupCopy = static_cast<CT_StandardItemGroup*>(groupToCopy->copy(childGroupModel, result));

                containerCopy->append(groupCopy);

                groupCopy->setParentGroup(&parentGroup);
                groupCopy->setParentContainer(containerCopy);
            }

            parentGroup.m_groupsCopied.insert(newKey, containerCopy);
        }

        return true;
    };

    parentGroupModel->visitGroups(visitor);
}

void CT_StandardItemGroup::staticCopyItemsFromXToBackupIfChildrenItemsAreStillPresent(const CT_AbstractResult* result,
                                                                                      CT_StandardItemGroup& parentGroup,
                                                                                      const ItemsCollection& mapSource)
{
    Q_UNUSED(result)

    DEF_CT_AbstractGroupModelOut* parentGroupModel = static_cast<DEF_CT_AbstractGroupModelOut*>(parentGroup.model());

    const auto visitor = [&](const CT_OutAbstractSingularItemModel* childItemModel) -> bool {

        const OutModelKeyType key = CT_StandardItemGroup::outModelCopyKey(childItemModel);
        const OutModelKeyType newKey = CT_StandardItemGroup::outModelKey(childItemModel);

        // if the unique index of the child item model is not present in the mapSource
        // it is because it was a new item that will be added when step compute
        ItemContainerType* containerSource = mapSource.value(key, nullptr);

        // if child item model is still present
        if(containerSource != nullptr)
            parentGroup.m_itemsCopied.insert(newKey, containerSource->copy()); // when copy => autoDelete of the container is set to false

        return true;
    };

    parentGroupModel->visitItems(visitor);
}

CT_StandardItemGroup::IChildrensIteratorQtStylePtr CT_StandardItemGroup::createQtStyleIteratorForChildrensThatUseOutModel(const CT_OutAbstractModel* outModel) const
{
    // is a group ?
    if(dynamic_cast<const CT_OutAbstractGroupModel*>(outModel) != nullptr) {
        QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessGroup));

        GroupContainerType* groupContainer = groupContainerWithOutModel(outModel);

        if(groupContainer != nullptr)
            return IChildrensIteratorQtStylePtr(new GroupQtIterator(groupContainer->begin(), groupContainer->end()));

        return IChildrensIteratorQtStylePtr(nullptr);
    }

    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    CT_AbstractSingularItemDrawable* item = internalSingularItemWithOutModel(outModel);

    if(item != nullptr)
        return IChildrensIteratorQtStylePtr(new ItemQtIterator(item));

    return IChildrensIteratorQtStylePtr(nullptr);
}

CT_AbstractSingularItemDrawable*CT_StandardItemGroup::internalSingularItemWithOutModel(const CT_OutAbstractModel* outModel) const
{
    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::internalSingularItemWithOutModel" << ", " << "outModel == nullptr";}

    Q_ASSERT(static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() == model()->uniqueIndex());
    if (static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() != model()->uniqueIndex()) {qDebug() << "CT_StandardItemGroup::internalSingularItemWithOutModel" << ", " << "static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() != model()->uniqueIndex()";}

    QMutexLocker locker(const_cast<QMutex*>(m_lockAccessTool.m_mutexAccessItem));

    const ItemContainerType* ct = itemContainerWithOutModel(outModel);

    return ((ct == nullptr) || ct->willBeRemovedLater()) ? nullptr : ct->item();
}

CT_StandardItemGroup::ItemContainerType* CT_StandardItemGroup::itemContainerWithOutModel(const CT_OutAbstractModel* outModel) const
{
    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::itemContainerWithOutModel" << ", " << "outModel == nullptr";}

    const OutModelKeyType key = CT_StandardItemGroup::outModelKey(outModel);

    ItemContainerType* ct = m_newItemsAdded.value(key, nullptr);

    if(ct == nullptr)
        ct = m_itemsCopied.value(key, nullptr);

    return ct;
}

CT_StandardItemGroup::GroupContainerType* CT_StandardItemGroup::groupContainerWithOutModel(const CT_OutAbstractModel* outModel) const
{
    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CT_StandardItemGroup::groupContainerWithOutModel" << ", " << "outModel == nullptr";}

    const OutModelKeyType key = CT_StandardItemGroup::outModelKey(outModel);

    GroupContainerType* groupContainer = m_newGroupsAdded.value(key, nullptr);

    if(groupContainer == nullptr)
        groupContainer = m_groupsCopied.value(key, nullptr);

    return groupContainer;
}

template<>
CT_StandardItemGroup::GroupContainerType* CT_StandardItemGroup::containerWithOutModel(const CT_OutAbstractModel* outModel) const { return groupContainerWithOutModel(outModel); }

template<>
CT_StandardItemGroup::ItemContainerType* CT_StandardItemGroup::containerWithOutModel(const CT_OutAbstractModel* outModel) const { return itemContainerWithOutModel(outModel); }

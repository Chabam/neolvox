#include "ct_outabstractmodel.h"

CT_OutAbstractModel::CT_OutAbstractModel(const QString& displayableName) :
    SuperClass(displayableName),
    m_uniqueIndex(-1),
    m_originalModel(nullptr),
    m_realResult(nullptr),
    m_item(nullptr)
{
}

CT_OutAbstractModel::CT_OutAbstractModel(const CT_OutAbstractModel& other) : SuperClass(other),
    m_uniqueIndex(other.m_uniqueIndex),
    m_originalModel(nullptr), // original model not set here because we will connect to signal of the original model in method "setOriginalModel"
    m_realResult(other.m_realResult),
    m_item(nullptr) // item not copied !
{
    setOriginalModel(const_cast<CT_OutAbstractModel*>(&other));
}

CT_OutAbstractModel::~CT_OutAbstractModel()
{
    deleteAndClearItem();
}

void CT_OutAbstractModel::setUniqueIndex(const UniqueIndexType& uniqueIndex)
{
    m_uniqueIndex = uniqueIndex;
}

CT_OutAbstractModel::UniqueIndexType CT_OutAbstractModel::uniqueIndex() const
{
    return m_uniqueIndex;
}

CT_OutAbstractModel* CT_OutAbstractModel::recursiveOriginalModel() const
{
    if(m_originalModel != nullptr)
        return m_originalModel->recursiveOriginalModel();

    return const_cast<CT_OutAbstractModel*>(this);
}

CT_OutAbstractModel* CT_OutAbstractModel::originalModel() const
{
    if(m_originalModel != nullptr)
        return m_originalModel;

    return const_cast<CT_OutAbstractModel*>(this);
}

CT_OutAbstractModel* CT_OutAbstractModel::recursiveOriginalModelWithAResult() const
{
    CT_OutAbstractModel* lastModel = nullptr;
    CT_OutAbstractModel* oModel = const_cast<CT_OutAbstractModel*>(this);

    do
    {
        if(oModel->m_realResult != nullptr)
            lastModel = oModel;

        oModel = oModel->m_originalModel;
    }while(oModel != nullptr);

    return lastModel;
}

CT_OutAbstractModel* CT_OutAbstractModel::recursiveOriginalModelWithAStep() const
{
    CT_OutAbstractModel* lastModel = nullptr;
    CT_OutAbstractModel* oModel = const_cast<CT_OutAbstractModel*>(this);

    do
    {
        if(oModel->step() != nullptr)
            lastModel = oModel;

        oModel = oModel->m_originalModel;
    }while(oModel != nullptr);

    return lastModel;
}

bool CT_OutAbstractModel::isVisible() const
{
    return !m_visibleInDocuments.isEmpty() || [this]() -> bool {
        const CT_OutAbstractModel* oModel = this->recursiveOriginalModelWithAResult();

        if((oModel != nullptr) && (oModel != this))
            return oModel->isVisible();

        return false;
    }();
}

bool CT_OutAbstractModel::isVisibleInDocument(const IDocumentForModel* doc) const
{
    return (m_visibleInDocuments.value(const_cast<IDocumentForModel*>(doc), 0) > 0) || [this, &doc]() -> bool {
        const CT_OutAbstractModel* oModel = this->recursiveOriginalModelWithAResult();

        if((oModel != nullptr) && (oModel != this))
            return oModel->isVisibleInDocument(doc);

        return false;
    }();
}

IPrototypeForModel* CT_OutAbstractModel::prototype() const
{
    return m_item;
}

IResultForModel* CT_OutAbstractModel::result() const
{
    return m_realResult;
}

bool CT_OutAbstractModel::recursiveVisitOutChildrens(const CT_OutAbstractModel::OutChildrenVisitor& visitor) const
{
    auto visitorAdapter = [&visitor](const CT_AbstractModel* child) -> bool {
        return visitor(static_cast<const CT_OutAbstractModel*>(child));
    };

    return recursiveVisitChildrens(visitorAdapter);
}

bool CT_OutAbstractModel::visitOutChildrens(const CT_OutAbstractModel::OutChildrenVisitor& visitor) const
{
    auto visitorAdapter = [&visitor](const CT_AbstractModel* child) -> bool {
        return visitor(static_cast<const CT_OutAbstractModel*>(child));
    };

    return visitChildrens(visitorAdapter);
}

void CT_OutAbstractModel::setResult(const IResultForModel* res)
{
    m_realResult = const_cast<IResultForModel*>(res);

    const auto visitor = [&res](const CT_AbstractModel* child) -> bool {
        const_cast<CT_OutAbstractModel*>(static_cast<const CT_OutAbstractModel*>(child))->setResult(res);
        return true;
    };

    visitChildrens(visitor);
}

void CT_OutAbstractModel::setOriginalModel(const CT_OutAbstractModel* model)
{
    if(m_originalModel != nullptr)
        disconnect(m_originalModel, nullptr, this, nullptr);

    m_originalModel = const_cast<CT_OutAbstractModel*>(model);

    if(m_originalModel != nullptr)
    {
        connect(m_originalModel, SIGNAL(visibilityChanged(bool)), this, SIGNAL(visibilityChanged(bool)), Qt::DirectConnection);
        connect(m_originalModel, SIGNAL(destroyed()), this, SLOT(originalModelDestroyed()), Qt::DirectConnection);
    }
}

void CT_OutAbstractModel::setPrototype(IPrototypeForModel* item)
{
    deleteAndClearItem();
    m_item = item;

    if(m_item != nullptr)
        m_item->prototypeToolForModel()->setModel(this);
}

void CT_OutAbstractModel::deleteAndClearItem()
{
    if(m_item != nullptr)
        m_item->prototypeToolForModel()->setModel(nullptr);

    delete m_item;
    m_item = nullptr;
}

void CT_OutAbstractModel::incrementVisibilityInDocument(const IDocumentForModel* doc)
{
    bool empty = m_visibleInDocuments.isEmpty();

    IDocumentForModel* d = const_cast<IDocumentForModel*>(doc);

    const int val = m_visibleInDocuments.value(d, 0) + 1;

    m_visibleInDocuments.insert(d, val);

    if(val == 1)
        emit visibilityInDocumentChanged(doc, true);

    if(m_visibleInDocuments.isEmpty() != empty)
        emit visibilityChanged(true);
}

void CT_OutAbstractModel::decrementVisibilityInDocument(const IDocumentForModel* doc)
{
    bool empty = m_visibleInDocuments.isEmpty();

    IDocumentForModel* d = const_cast<IDocumentForModel*>(doc);

    const int val = m_visibleInDocuments.value(d, 0) - 1;

    if(val > 0)
        m_visibleInDocuments.insert(d, val);
    else
        m_visibleInDocuments.remove(d);

    if(val == 0)
        emit visibilityInDocumentChanged(doc, false);

    if(m_visibleInDocuments.isEmpty() != empty)
        emit visibilityChanged(false);
}

bool CT_OutAbstractModel::recursiveFinalize()
{
    if(!finalize())
        return false;

    const auto visitor = [](const CT_OutAbstractModel* child) -> bool {
        CT_OutAbstractModel* c = const_cast<CT_OutAbstractModel*>(child);

        if(!c->finalize())
            return false;

        return true;
    };

    return recursiveVisitOutChildrens(visitor);
}

bool CT_OutAbstractModel::finalize()
{
    return true;
}

void CT_OutAbstractModel::originalModelDestroyed()
{
    m_originalModel = nullptr;
}

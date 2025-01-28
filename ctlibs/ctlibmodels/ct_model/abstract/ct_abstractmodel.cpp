#include "ct_abstractmodel.h"

CT_AbstractModel::CT_AbstractModel(const QString& displayableName) :
    m_displayableName(displayableName),
    m_shortDescription(displayableName),
    m_parentModel(nullptr),
    m_step(nullptr)
{
}

CT_AbstractModel::CT_AbstractModel(const CT_AbstractModel& other) : QObject( nullptr )
{
    m_displayableName = other.m_displayableName;
    m_shortDescription = other.m_shortDescription;
    m_fullDescription = other.m_fullDescription;
    m_parentModel = nullptr;
    m_step = other.m_step;
}

CT_AbstractModel::~CT_AbstractModel()
{
}

QString CT_AbstractModel::shortDescription() const
{
    return m_shortDescription.isEmpty() ? displayableName() : m_shortDescription;
}

QString CT_AbstractModel::detailledDescription() const
{
    return m_fullDescription.isEmpty() ? shortDescription() : m_fullDescription;
}

QString CT_AbstractModel::displayableName() const
{
    return m_displayableName;
}

void CT_AbstractModel::setDisplayableName(const QString& displayableName)
{
    m_displayableName = displayableName;
}

void CT_AbstractModel::setShortDescription(const QString& shortDescription)
{
    m_shortDescription = shortDescription;
}

void CT_AbstractModel::setDetailledDescription(const QString& fullDescription)
{
    m_fullDescription = fullDescription;
}

bool CT_AbstractModel::recursiveVisitChildrens(const CT_AbstractModel::ChildrenVisitor& visitor) const
{
    if(isEmpty())
        return true;

    const auto visitorAdapter = [&visitor](const CT_AbstractModel* child) -> bool {
        if(!visitor(child))
            return false;

        return child->recursiveVisitChildrens(visitor);
    };

    return visitChildrens(visitorAdapter);
}

bool CT_AbstractModel::existInTree(const CT_AbstractModel* model) const
{
    bool exist = false;

    auto visitor = [&](const CT_AbstractModel* m) -> bool {
        exist = (m == model);
        return !exist;
    };

    rootModel()->recursiveVisitChildrens(visitor);

    return exist;
}

CT_AbstractModel* CT_AbstractModel::parentModel() const
{
    return m_parentModel;
}

CT_AbstractModel* CT_AbstractModel::rootModel() const
{
    CT_AbstractModel* root = const_cast<CT_AbstractModel*>(this);

    while(root->parentModel() != nullptr)
        root = root->parentModel();

    return root;
}

IStepForModel* CT_AbstractModel::step() const
{
    return m_step;
}

void CT_AbstractModel::setParentModel(const CT_AbstractModel* parent)
{
    m_parentModel = const_cast<CT_AbstractModel*>(parent);
}

void CT_AbstractModel::setStep(const IStepForModel* step)
{
    m_step = const_cast<IStepForModel*>(step);

    auto visitor = [&](const CT_AbstractModel* model) -> bool {
        const_cast<CT_AbstractModel*>(model)->setStep(step);
        return true;
    };

    recursiveVisitChildrens(visitor);
}

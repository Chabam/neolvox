#include "ct_inresultmodelgroup.h"

#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"
#include "ct_model/inModel/ct_inzeroormoregroupmodel.h"
#include "ct_model/inModel/ct_instdgroupmodel.h"
#include "ct_model/inModel/ct_instdsingularitemmodel.h"
#include "ct_model/inModel/ct_instditemattributemodel.h"

#include "ct_model/outModel/abstract/ct_outabstractresultmodelgroup.h"

CT_InResultModelGroup::CT_InResultModelGroup(const QString& displayableName,
                                             const QString& shortDescription,
                                             bool recursive,
                                             quint8 minimumNumberOfPossibilityToSelect,
                                             int maximumNumberOfPossibilityThatCanBeSelected) : SuperClass(displayableName),
    m_rootGroup(nullptr)
{
    setShortDescription(shortDescription.isEmpty() ? displayableName : shortDescription);
    setRecursive(recursive);
    setMinimumNumberOfPossibilityToSelect(minimumNumberOfPossibilityToSelect);
    setMaximumNumberOfPossibilityThatCanBeSelected(maximumNumberOfPossibilityThatCanBeSelected);
}

CT_InResultModelGroup::CT_InResultModelGroup(const CT_InResultModelGroup& other,
                                             const bool& withPossibilities) : SuperClass(other),
    m_rootGroup((other.m_rootGroup != nullptr) ? static_cast<DEF_CT_AbstractGroupModelIn*>(other.m_rootGroup->copy(withPossibilities)) : nullptr)
{
    if(withPossibilities)
        staticCopyPossibilitiesToModel(&other, this);
}

CT_InResultModelGroup::~CT_InResultModelGroup()
{
    delete m_rootGroup;
}

DEF_CT_AbstractGroupModelIn* CT_InResultModelGroup::rootGroup() const
{
    return m_rootGroup;
}

void CT_InResultModelGroup::setRootGroup(DEF_CT_AbstractGroupModelIn* rootGroup)
{
    delete m_rootGroup;

    m_rootGroup = rootGroup;
    m_rootGroup->setParentModel(this);
}

CT_InZeroOrMoreGroupModel* CT_InResultModelGroup::setZeroOrMoreRootGroup(const QString& groupType,
                                                                         const QString& groupNameFromType)
{
    CT_InZeroOrMoreGroupModel* model = new CT_InZeroOrMoreGroupModel(groupType, groupNameFromType);
    setRootGroup(model);

    return model;
}

CT_InStdGroupModel* CT_InResultModelGroup::setStdRootGroup(const QString& groupType,
                                                           const QString& groupNameFromType,
                                                           const QString& displayableName,
                                                           const QString& shortDescription,
                                                           const QString& detailledDescription,
                                                           const quint8& minPossibility,
                                                           const int& maxPossibility)
{
    CT_InStdGroupModel* model = new CT_InStdGroupModel(groupType,
                                                       groupNameFromType,
                                                       displayableName,
                                                       shortDescription,
                                                       detailledDescription,
                                                       minPossibility,
                                                       maxPossibility);

    setRootGroup(model);

    return model;
}

CT_InStdGroupModel* CT_InResultModelGroup::addStdGroupModel(DEF_CT_AbstractGroupModelIn* parentGroup,
                                                            const QString& groupType,
                                                            const QString& groupNameFromType,
                                                            const QString& displayableName,
                                                            const QString& shortDescription,
                                                            const QString& detailledDescription,
                                                            const quint8& minPossibility,
                                                            const int& maxPossibility)
{
    MODELS_ASSERT(parentGroup != nullptr);
    if (parentGroup == nullptr) {qDebug() << "CT_InResultModelGroup::addStdGroupModel" << ", " <<  "parentGroup == nullptr"; return nullptr;}

    MODELS_ASSERT(!groupType.isEmpty());
    if (groupType.isEmpty()) {qDebug() << "CT_InResultModelGroup::addStdGroupModel" << ", " <<  "groupType.isEmpty()"; return nullptr;}

    MODELS_ASSERT(!displayableName.isEmpty());
    if (displayableName.isEmpty()) {qDebug() << "CT_InResultModelGroup::addStdGroupModel" << ", " <<  "displayableName.isEmpty()"; return nullptr;}

    CT_InStdGroupModel* newModel = new CT_InStdGroupModel(groupType,
                                                          groupNameFromType,
                                                          displayableName,
                                                          shortDescription,
                                                          detailledDescription,
                                                          minPossibility,
                                                          maxPossibility);

    parentGroup->addGroup(newModel);

    return newModel;
}

CT_InStdSingularItemModel* CT_InResultModelGroup::addStdItemModel(DEF_CT_AbstractGroupModelIn* parentGroup,
                                                                  const QString& itemType,
                                                                  const QString& itemNameFromType,
                                                                  const QString& displayableName,
                                                                  const QString& shortDescription,
                                                                  const QString& detailledDescription,
                                                                  const quint8& minPossibility,
                                                                  const int& maxPossibility)
{
    MODELS_ASSERT(parentGroup != nullptr);
    if (parentGroup == nullptr) {qDebug() << "CT_InResultModelGroup::addStdItemModel" << ", " <<  "parentGroup == nullptr"; return nullptr;}

    MODELS_ASSERT(!itemType.isEmpty());
    if (itemType.isEmpty()) {qDebug() << "CT_InResultModelGroup::addStdItemModel" << ", " <<  "itemType.isEmpty()"; return nullptr;}

    MODELS_ASSERT(!displayableName.isEmpty());
    if (displayableName.isEmpty()) {qDebug() << "CT_InResultModelGroup::addStdItemModel" << ", " <<  "displayableName.isEmpty()"; return nullptr;}

    CT_InStdSingularItemModel* newModel = new CT_InStdSingularItemModel(itemType,
                                                                        itemNameFromType,
                                                                        displayableName,
                                                                        shortDescription,
                                                                        detailledDescription,
                                                                        minPossibility,
                                                                        maxPossibility);

    parentGroup->addItem(newModel);

    return newModel;
}

CT_InStdItemAttributeModel* CT_InResultModelGroup::addStdItemAttributeModel(DEF_CT_AbstractItemDrawableModelIn* parentItem,
                                                                            const int valueType,
                                                                            const QList<QString>& categoriesType,
                                                                            const QString& displayableName,
                                                                            const QString& shortDescription,
                                                                            const QString& detailledDescription,
                                                                            const quint8& minPossibility,
                                                                            const int& maxPossibility)
{
    MODELS_ASSERT(parentItem != nullptr);
    if (parentItem == nullptr) {qDebug() << "CT_InResultModelGroup::addStdItemAttributeModel" << ", " <<  "parentItem == nullptr"; return nullptr;}

    MODELS_ASSERT(!categoriesType.isEmpty());
    if (categoriesType.isEmpty()) {qDebug() << "CT_InResultModelGroup::addStdItemAttributeModel" << ", " <<  "categoriesType.isEmpty()"; return nullptr;}

    MODELS_ASSERT(!displayableName.isEmpty());
    if (displayableName.isEmpty()) {qDebug() << "CT_InResultModelGroup::addStdItemAttributeModel" << ", " <<  "displayableName.isEmpty()"; return nullptr;}

    CT_InStdItemAttributeModel* newModel = new CT_InStdItemAttributeModel(valueType, categoriesType, displayableName, shortDescription, detailledDescription, minPossibility, maxPossibility);

    parentItem->addItemAttribute(newModel);

    return newModel;
}

bool CT_InResultModelGroup::visitChildrens(const CT_AbstractModel::ChildrenVisitor& visitor) const
{
    if(rootGroup() != nullptr)
        return visitor(rootGroup());

    return true;
}

bool CT_InResultModelGroup::isEmpty() const
{
    return (rootGroup() == nullptr);
}

void CT_InResultModelGroup::saveSettings(SettingsWriterInterface& writer) const
{
    SuperClass::saveSettings(writer);

    writer.addParameter(this, "IsRecursive", isRecursive());
}

bool CT_InResultModelGroup::restoreSettings(SettingsReaderInterface& reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    QVariant value;

    if(!reader.parameter(this, "IsRecursive", value))
        return false;

    const bool recursive = value.toBool();

    if(isRecursive() != recursive)
        return false;

    // TODO : MK 08.11.18 check if this is necessary and why i have coded this ???
    /*const bool recursive = value.toBool();

    if(isRecursive() != recursive)
    {
        setRecursiveWithoutUseForceRecursivity(recursive);

        CT_InAbstractResultModel *om = static_cast<CT_InAbstractResultModel*>(recursiveOriginalModel());

        if(om != this)
            staticSetRecursiveWithoutUseForceRecursivity(om, recursive);

        recursiveClearPossibilitiesSaved();

        IStepForModel* sp = step();
        bool continueLoop = true;

        while(continueLoop
              && (sp != nullptr))
        {
            sp = sp->parentStep();

            if(sp != nullptr)
            {
                // get all OUTPUT models of this step that represent results
                QList<CT_OutAbstractResultModel*> outputResultModels = sp->getOutResultModels();

                // for each models
                for(const CT_OutAbstractResultModel* outputResultModel : outputResultModels)
                    recursiveFindAllPossibilitiesInModel(*outputResultModel);

                continueLoop = recursive;
            }
        }
    }*/

    return true;
}

CT_InAbstractModel* CT_InResultModelGroup::copy(bool withPossibilities) const
{
    return new CT_InResultModelGroup(*this, withPossibilities);
}

bool CT_InResultModelGroup::canBeComparedWith(const CT_OutAbstractModel& model) const
{
    // if the OUTPUT model represent a CT_ResultGroup it's ok !
    return (dynamic_cast<const CT_OutAbstractResultModelGroup*>(&model) != nullptr);
}

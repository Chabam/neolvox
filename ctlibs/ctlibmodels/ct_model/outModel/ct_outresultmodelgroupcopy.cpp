#include "ct_model/outModel/ct_outresultmodelgroupcopy.h"

#include "ct_model/inModel/ct_inresultmodelgrouptocopy.h"
#include "ct_model/inModel/tools/ct_instdresultmodelpossibility.h"

CT_OutResultModelGroupCopy::CT_OutResultModelGroupCopy(const CT_InResultModelGroupToCopy* inResultModel,
                                                       const QString& resultName,
                                                       const QString& modelDisplayableName,
                                                       const QString& modelShortDescription) : SuperClass(modelDisplayableName),
    m_inResultModel(const_cast<CT_InResultModelGroupToCopy*>(inResultModel)),
    m_rootGroupForModification(nullptr)
{

    MODELS_ASSERT(m_inResultModel != nullptr);
    if (m_inResultModel == nullptr) {qDebug() << "CT_OutResultModelGroupCopy::CT_OutResultModelGroupCopy" << ", " <<  "m_inResultModel == nullptr"; return;}

    MODELS_ASSERT(m_inResultModel->nPossibilitySaved() == 1);
    if (m_inResultModel->nPossibilitySaved() != 1) {qDebug() << "CT_OutResultModelGroupCopy::CT_OutResultModelGroupCopy" << ", " <<  "m_inResultModel->nPossibilitySaved() != 1"; return;}

    MODELS_ASSERT(m_inResultModel->nPossibilitySelected() == 1);
    if (m_inResultModel->nPossibilitySelected() != 1) {qDebug() << "CT_OutResultModelGroupCopy::CT_OutResultModelGroupCopy" << ", " <<  "m_inResultModel->nPossibilitySelected() != 1"; return;}

    MODELS_ASSERT(dynamic_cast<CT_InStdResultModelPossibility*>(m_inResultModel->getPossibilitiesSelected().first()) != nullptr);
    if (dynamic_cast<CT_InStdResultModelPossibility*>(m_inResultModel->getPossibilitiesSelected().first()) == nullptr) {qDebug() << "CT_OutResultModelGroupCopy::CT_OutResultModelGroupCopy" << ", " <<  "dynamic_cast<CT_InStdResultModelPossibility*>(m_inResultModel->getPossibilitiesSelected().first()) == nullptr"; return;}

    // get the possibility linked with the input result model
    const CT_InStdResultModelPossibility* resultPossibility = static_cast<CT_InStdResultModelPossibility*>(m_inResultModel->getPossibilitiesSelected().first());

    MODELS_ASSERT(dynamic_cast<CT_OutAbstractResultModelGroup*>(resultPossibility->outResultModel()) != nullptr);
    if (dynamic_cast<CT_OutAbstractResultModelGroup*>(resultPossibility->outResultModel()) == nullptr) {qDebug() << "CT_OutResultModelGroupCopy::CT_OutResultModelGroupCopy" << ", " <<  "dynamic_cast<CT_OutAbstractResultModelGroup*>(resultPossibility->outResultModel()) == nullptr"; return;}

    // get the output result model of this possibility
    const CT_OutAbstractResultModelGroup* originalOutResultModel = static_cast<CT_OutAbstractResultModelGroup*>(resultPossibility->outResultModel());

    MODELS_ASSERT(originalOutResultModel != nullptr);
    if (originalOutResultModel == nullptr) {qDebug() << "CT_OutResultModelGroupCopy::CT_OutResultModelGroupCopy" << ", " <<  "originalOutResultModel == nullptr"; return;}

    // copy the root group of this possibility because it can be modified by the developper in the step
    m_rootGroupForModification = originalOutResultModel->rootGroup();

    if(m_rootGroupForModification != nullptr) {
        m_rootGroupForModification = static_cast<DEF_CT_AbstractGroupModelOut*>(m_rootGroupForModification->copy());
        m_rootGroupForModification->setParentModel(this);
    }

    setResultName(resultName);
    setShortDescription(modelShortDescription);
}

CT_OutResultModelGroupCopy::~CT_OutResultModelGroupCopy()
{
    delete m_rootGroupForModification;
}

CT_OutResultModelGroupCopy::CT_OutResultModelGroupCopy(const CT_OutResultModelGroupCopy& other) : SuperClass(other),
    m_inResultModel(nullptr), // can be set to nullptr because the method "createResult" must never be called if it was a copy
    m_rootGroupForModification(nullptr)
{
    if(other.m_rootGroupForModification != nullptr) {
        m_rootGroupForModification = static_cast<DEF_CT_AbstractGroupModelOut*>(other.m_rootGroupForModification->copy());
        m_rootGroupForModification->setParentModel(this);
    }
}

void CT_OutResultModelGroupCopy::setRootGroup(DEF_CT_AbstractGroupModelOut* rootGroup)
{
    delete m_rootGroupForModification;
    m_rootGroupForModification = rootGroup;

    if(m_rootGroupForModification != nullptr)
        m_rootGroupForModification->setParentModel(this);
}

DEF_CT_AbstractGroupModelOut* CT_OutResultModelGroupCopy::rootGroup() const
{
    return m_rootGroupForModification;
}

void CT_OutResultModelGroupCopy::removeGroup(const DEF_CT_AbstractGroupModelIn* inGroupToRemove)
{
    QSet<DEF_CT_AbstractGroupModelOut*> groupsToRemove;

    const auto visitor = [&groupsToRemove](CT_OutAbstractModel* outModelThatCanBeModified) {
        groupsToRemove.insert(static_cast<DEF_CT_AbstractGroupModelOut*>(outModelThatCanBeModified));
    };

    internalVisitOutModelThatCanBeModified(inGroupToRemove,
                                           visitor);

    QMutableSetIterator<DEF_CT_AbstractGroupModelOut*> it(groupsToRemove);

    while(it.hasNext()) {
        DEF_CT_AbstractGroupModelOut* parentGroup = it.next()->parentGroup();

        if(parentGroup != nullptr) {

            while(parentGroup != nullptr) {

                if(groupsToRemove.contains(parentGroup)) {
                    it.remove();
                    break;
                }

                parentGroup = parentGroup->parentGroup();
            }
        }
    }

    it.toFront();

    while(it.hasNext()) {
        DEF_CT_AbstractGroupModelOut* group = it.next();
        DEF_CT_AbstractGroupModelOut* parentGroup = group->parentGroup();

        if(parentGroup != nullptr)
            parentGroup->removeGroup(group);
        else {
            CT_OutAbstractResultModelGroup* resultGroup = dynamic_cast<CT_OutAbstractResultModelGroup*>(group->parentModel());
            resultGroup->setRootGroup(nullptr);

            MODELS_ASSERT(!it.hasNext());
            if (it.hasNext()) {qDebug() << "CT_OutResultModelGroupCopy::removeGroup" << ", " <<  "it.hasNext()"; return;}
        }
    }
}

void CT_OutResultModelGroupCopy::removeItem(const DEF_CT_AbstractItemDrawableModelIn* inItemToRemove)
{
    QSet<DEF_CT_AbstractItemDrawableModelOut*> itemsToRemove;

    const auto visitor = [&itemsToRemove](CT_OutAbstractModel* outModelThatCanBeModified) {
        itemsToRemove.insert(static_cast<DEF_CT_AbstractItemDrawableModelOut*>(outModelThatCanBeModified));
    };

    internalVisitOutModelThatCanBeModified(inItemToRemove,
                                           visitor);

    QSetIterator<DEF_CT_AbstractItemDrawableModelOut*> it(itemsToRemove);

    while(it.hasNext()) {
        DEF_CT_AbstractItemDrawableModelOut* item = it.next();
        DEF_CT_AbstractGroupModelOut* parentGroup = item->parentGroup();

        parentGroup->removeItem(item);
    }
}

CT_OutAbstractModel* CT_OutResultModelGroupCopy::recursiveSearchTheModelThatWasACopiedModelFromThisOriginalModel(const CT_OutAbstractModel* originalModel) const
{
    Q_ASSERT(originalModel != nullptr);
    if (originalModel == nullptr) {qDebug() << "CT_OutResultModelGroupCopy::recursiveSearchTheModelThatWasACopiedModelFromThisOriginalModel" << ", " <<  "originalModel == nullptr"; return nullptr;}

    const CT_OutAbstractModel* firstOriginalOutModel = originalModel->recursiveOriginalModel();

    CT_OutAbstractResultModel* rootModel = dynamic_cast<CT_OutAbstractResultModel*>(firstOriginalOutModel->rootModel());

    if((rootModel == firstOriginalOutModel) && (rootModel->recursiveOriginalModel() == this->recursiveOriginalModel()))
        return const_cast<CT_OutResultModelGroupCopy*>(this);

    CT_OutAbstractModel* copiedModel = nullptr;

    const auto visitor = [&copiedModel, &firstOriginalOutModel](const CT_OutAbstractModel* child) -> bool {
        if(child->recursiveOriginalModel() == firstOriginalOutModel) {
            copiedModel = const_cast<CT_OutAbstractModel*>(child);
            return false;
        }

        return true;
    };

    recursiveVisitOutChildrens(visitor);

    return copiedModel;
}

IResultForModel* CT_OutResultModelGroupCopy::createResult()
{
    MODELS_ASSERT(m_inResultModel != nullptr);
    if (m_inResultModel == nullptr) {qDebug() << "CT_OutResultModelGroupCopy::createResult" << ", " <<  "m_inResultModel == nullptr"; return nullptr;}

    MODELS_ASSERT(m_inResultModel->nPossibilitySaved() == 1);
    if (m_inResultModel->nPossibilitySaved() != 1) {qDebug() << "CT_OutResultModelGroupCopy::createResult" << ", " <<  "m_inResultModel->nPossibilitySaved() != 1"; return nullptr;}

    IResultForModel* res = nullptr;

    const auto visitor = [&res](const IResultForModel* result) -> bool {
        MODELS_ASSERT(res == nullptr);
        if (res != nullptr) {qDebug() << "CT_OutResultModelGroupCopy::createResult" << ", " <<  "res == nullptr"; return false;}

        res = const_cast<IResultForModel*>(result);
        return true;
    };

    // get the original result that match with the possibility selected in this input result model
    m_inResultModel->visitRealResultsThatMatchWithSelectedPossibilities(visitor);

    MODELS_ASSERT(res != nullptr);
    if (res == nullptr) {qDebug() << "CT_OutResultModelGroupCopy::createResult" << ", " <<  "res == nullptr"; return nullptr;}

    // create the copy
    res = res->resultToolForModel()->copyResult(this);
    res->resultToolForModel()->setResultName(resultName());

    this->setResult(res);

    return res;
}

CT_OutAbstractResultModel* CT_OutResultModelGroupCopy::copy() const
{
    return new CT_OutResultModelGroupCopy(*this);
}

CT_InAbstractModel* CT_OutResultModelGroupCopy::recursiveSearchTheInModelThatWasACopiedModelFromThisOriginalInModel(const CT_InAbstractModel* originalModel) const
{
    MODELS_ASSERT(m_inResultModel != nullptr);
    if (m_inResultModel == nullptr) {qDebug() << "CT_OutResultModelGroupCopy::recursiveSearchTheInModelThatWasACopiedModelFromThisOriginalInModel" << ", " <<  "m_inResultModel == nullptr"; return nullptr;}

    return m_inResultModel->recursiveSearchTheModelThatWasACopiedModelFromThisOriginalModel(originalModel);
}

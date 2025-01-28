#include "ct_inabstractmodel.h"

#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"
#include "ct_model/outModel/abstract/ct_outabstractmodel.h"

//#define DEBUG_MODELS 2

#ifdef DEBUG_MODELS
#include <QDebug>

QString DEBUG_STR_FOR_MODELS;

#define DEBUG_APPEND QString("\t")
#define BEGIN_RECURSIVE_DEBUG DEBUG_STR_FOR_MODELS += DEBUG_APPEND;
#define END_RECURSIVE_DEBUG DEBUG_STR_FOR_MODELS.remove(DEBUG_STR_FOR_MODELS.size()-DEBUG_APPEND.size(), DEBUG_APPEND.size());

#define DEBUG_MODEL_SEARCH(strToPrint) qDebug() << DEBUG_STR_FOR_MODELS << this << " => " << strToPrint;

    #if DEBUG_MODELS > 1
        #define DEBUG_MODEL_SEARCH_PRECISE_1(strToPrint) DEBUG_MODEL_SEARCH(strToPrint);
    #else
        #define DEBUG_MODEL_SEARCH_PRECISE_1(strToPrint) ;
    #endif

#else
    #define BEGIN_RECURSIVE_DEBUG ;
    #define END_RECURSIVE_DEBUG ;
    #define DEBUG_MODEL_SEARCH(strToPrint) ;
    #define DEBUG_MODEL_SEARCH_PRECISE_1(strToPrint) ;
#endif

CT_InAbstractModel::CT_InAbstractModel(const QString& displayableName) : SuperClass(displayableName),
    m_originalModel(nullptr)
{
    m_possibilitiesGroup = new CT_InStdModelPossibilitySelectionGroup();
    m_possibilitiesGroup->setEnable(true);

    // by default this model doesn't need output model (min = 0 and max = 0)
    m_possibilitiesGroup->setMinimumNumberOfPossibilityThatMustBeSelected(0);
    m_possibilitiesGroup->setMaximumNumberOfPossibilityThatCanBeSelected(0);
}

CT_InAbstractModel::CT_InAbstractModel(const CT_InAbstractModel& other) : SuperClass(other),
    m_originalModel(nullptr)
{
    // possibilities was not copied because the copy constructor of CT_InStdModelPossibilityGroup doesn't copy possibilities
    m_possibilitiesGroup = new CT_InStdModelPossibilitySelectionGroup(*other.m_possibilitiesGroup);

    setOriginalModel(&other);
}

CT_InAbstractModel::~CT_InAbstractModel()
{
    clearPossibilitiesSaved();

    delete m_possibilitiesGroup;
}

quint8 CT_InAbstractModel::minimumNumberOfPossibilityToSelect() const
{
    return m_possibilitiesGroup->minimumNumberOfPossibilityThatMustBeSelected();
}

int CT_InAbstractModel::maximumNumberOfPossibilityThatCanBeSelected() const
{
    return m_possibilitiesGroup->maximumNumberOfPossibilityThatCanBeSelected();
}

bool CT_InAbstractModel::isOptionnal() const
{
    return (minimumNumberOfPossibilityToSelect() == 0) || !needOutputModel();
}

bool CT_InAbstractModel::isObligatory() const
{
    return needOutputModel() && (minimumNumberOfPossibilityToSelect() > 0);
}

bool CT_InAbstractModel::recursiveAtLeastOneChildrenOrThisIsObligatory() const
{
    if(isObligatory())
        return true;

    bool o = false;

    recursiveVisitInChildrens([&o](const CT_InAbstractModel* model) -> bool {
        o = model->isObligatory();
        return !o;
    });

    return o;
}

bool CT_InAbstractModel::needOutputModel() const
{
    return !((minimumNumberOfPossibilityToSelect() == 0) && (maximumNumberOfPossibilityThatCanBeSelected() == 0));
}

bool CT_InAbstractModel::visitInChildrens(const CT_InAbstractModel::InChildrenVisitor& visitor) const
{
    auto visitorAdapter = [&visitor](const CT_AbstractModel* child) -> bool {
        return visitor(static_cast<const CT_InAbstractModel*>(child));
    };

    return visitChildrens(visitorAdapter);
}

bool CT_InAbstractModel::recursiveVisitInChildrens(const CT_InAbstractModel::InChildrenVisitor& visitor) const
{
    if(isEmpty())
        return true;

    const auto visitorAdapter = [&visitor](const CT_InAbstractModel* child) -> bool {
        if(!visitor(child))
            return false;

        return child->recursiveVisitInChildrens(visitor);
    };

    return visitInChildrens(visitorAdapter);
}

bool CT_InAbstractModel::recursiveVisitPossibilities(const PossibilityVisitor& visitor) const
{
    if(!visitPossibilities(visitor))
        return false;

    const auto childVisitor = [&visitor](const CT_InAbstractModel* model) -> bool {
        return model->visitPossibilities(visitor);
    };

    return recursiveVisitInChildrensOrInModelOfPossibilities(childVisitor);
}

bool CT_InAbstractModel::visitPossibilities(const PossibilityVisitor& visitor) const
{
    const auto visitorAdapter = [this, &visitor](const CT_InStdModelPossibility* possibility) -> bool {
        return visitor(this, possibility);
    };

    return m_possibilitiesGroup->visitPossibilities(visitorAdapter);
}

bool CT_InAbstractModel::visitSelectedPossibilities(const CT_InAbstractModel::PossibilityVisitor& visitor) const
{
    const auto visitorAdapter = [this, &visitor](const CT_InStdModelPossibility* possibility) -> bool {
        return visitor(this, possibility);
    };

    return m_possibilitiesGroup->visitSelectedPossibilities(visitorAdapter);
}

CT_InAbstractModel::Possibilities CT_InAbstractModel::savedPossibilities() const
{
    return m_possibilitiesGroup->savedPossibilities();
}

CT_InAbstractModel::Possibilities CT_InAbstractModel::selectedPossibilities() const
{
    return m_possibilitiesGroup->selectedPossibilities();
}

void CT_InAbstractModel::startSaveCycle()
{
    m_saveCycles.append(CT_InModelSaveCycle());
}

void CT_InAbstractModel::finishSaveCycle()
{
}

int CT_InAbstractModel::nSaveCycle() const
{
    return m_saveCycles.size();
}

bool CT_InAbstractModel::recursiveFindAllPossibilitiesInModel(const CT_OutAbstractModel& rootModel, bool mustClearPossibilitiesSaved)
{
    emit startFindNewPossibilities(this);

    if(mustClearPossibilitiesSaved)
        recursiveClearPossibilitiesSaved();

    DEBUG_MODEL_SEARCH("---------- recursiveFindAllPossibilitiesInModel ------------");
    return recursiveFindPossibilitiesInModel(rootModel) != 0;
}

bool CT_InAbstractModel::isAtLeastOnePossibilitySelected() const
{
    return nPossibilitySelected() > 0;
}

bool CT_InAbstractModel::recursiveIsAtLeastOnePossibilitySelected() const
{
    if(!isAtLeastOnePossibilitySelected())
        return false;

    const auto childVisitor = [](const CT_AbstractModel* model) -> bool {
        return static_cast<const CT_InAbstractModel*>(model)->isAtLeastOnePossibilitySelected();
    };

    return recursiveVisitChildrens(childVisitor);
}

bool CT_InAbstractModel::hasTheMinimumNumberOfPossibilityRequired(QStringList* errors) const
{
    // if this model not need possiblities we don't have to check
    if(isOptionnal())
        return true;

    const int nSaved = nPossibilitySaved();

    if(nSaved < int(minimumNumberOfPossibilityToSelect())) {
        if(errors != nullptr)
            errors->append(tr("Model %1 has %2 possibilities available on %3 minimum").arg(displayableName()).arg(nSaved).arg(minimumNumberOfPossibilityToSelect()));

        return false;
    }

    return true;
}

bool CT_InAbstractModel::recursiveHasTheMinimumNumberOfPossibilityRequired(QStringList* errors) const
{
    if(!hasTheMinimumNumberOfPossibilityRequired(errors))
        return false;

    const auto childVisitor = [&errors](const CT_InAbstractModel* model) -> bool {
        return model->recursiveHasTheMinimumNumberOfPossibilityRequired(errors);
    };

    return visitInChildrensOrInModelOfPossibilities(childVisitor);
}

bool CT_InAbstractModel::hasTheMinimumNumberOfSelectedPossibilityRequired(QStringList* errors) const
{
    // if this model not need possiblities we don't have to check
    if(isOptionnal())
        return true;

    const int nSelected = nPossibilitySelected();

    if(nSelected < int(minimumNumberOfPossibilityToSelect())) {
        if(errors != nullptr)
            errors->append(tr("Model %1 has %2 option(s) selected on %3 minimum (%4 available)").arg(displayableName()).arg(nSelected).arg(minimumNumberOfPossibilityToSelect()).arg(nPossibilitySaved()));

        return false;
    }

    return true;
}

bool CT_InAbstractModel::recursiveHasTheMinimumNumberOfSelectedPossibilityRequired(QStringList* errors) const
{
    if(!hasTheMinimumNumberOfSelectedPossibilityRequired(errors))
        return false;

    const auto childVisitor = [&errors](const CT_InAbstractModel* model) -> bool {
        return model->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired(errors);
    };

    return visitInChildrensOrInModelOfPossibilities(childVisitor);
}

int CT_InAbstractModel::nPossibilitySaved() const
{
    return m_possibilitiesGroup->nPossibility();
}

int CT_InAbstractModel::nPossibilitySelected() const
{
    return m_possibilitiesGroup->nPossibilitySelected();
}

CT_InStdModelPossibility* CT_InAbstractModel::possibilitySavedAt(const int& index) const
{
    return m_possibilitiesGroup->getPossibilities().at(index);
}

CT_InStdModelPossibility* CT_InAbstractModel::possibilitySelectedAt(const int& index) const
{
    return m_possibilitiesGroup->getPossibilitiesSelected().at(index);
}

QList<CT_InStdModelPossibility *> CT_InAbstractModel::getPossibilitiesSaved() const
{
    return m_possibilitiesGroup->getPossibilities();
}

QList<CT_InStdModelPossibility *> CT_InAbstractModel::getPossibilitiesSelected() const
{
    return m_possibilitiesGroup->getPossibilitiesSelected();
}

void CT_InAbstractModel::clearPossibilitiesSaved()
{
    while(deleteLastSaveCycle());
}

void CT_InAbstractModel::recursiveClearPossibilitiesSaved()
{
    clearPossibilitiesSaved();

    const auto visitor = [](const CT_InAbstractModel* model) -> bool{
        const_cast<CT_InAbstractModel*>(model)->recursiveClearPossibilitiesSaved();
        return true;
    };

    visitInChildrens(visitor);
}

void CT_InAbstractModel::saveSettings(SettingsWriterInterface& writer) const
{
    writer.addParameter(this, "ModelType", metaObject()->className());
    writer.addParameter(this, "MinimumNumberOfPossibilityToSelect", minimumNumberOfPossibilityToSelect());
    writer.addParameter(this, "MaximumNumberOfPossibilityThatCanBeSelected", maximumNumberOfPossibilityThatCanBeSelected());

    QListIterator<CT_InStdModelPossibility*> it(m_possibilitiesGroup->getPossibilities());

    while(it.hasNext())
    {
        CT_InStdModelPossibility* possibility = it.next();

        const int pID = writer.addParameter(this, "Possibility", "");
        writer.addParameterInfo(this, pID, "IsSelected", possibility->isSelected() ? true : false);
        writer.addParameterInfo(this, pID, "OutModelUniqueIndex", possibility->outModel()->uniqueIndex());
        writer.addParameterInfo(this, pID, "OutModelType", possibility->outModel()->metaObject()->className());
    }

    writer.beginWriteChildrens(this);

    const auto visitor = [&writer](const CT_InAbstractModel* model) -> bool{
        const_cast<CT_InAbstractModel*>(model)->saveSettings(writer);
        return true;
    };

    visitInChildrensOrInModelOfPossibilities(visitor);

    writer.endWriteChildrens(this);
}

bool CT_InAbstractModel::restoreSettings(SettingsReaderInterface& reader)
{
    QVariant value;

    if(!reader.parameter(this, "ModelType", value) || value.toString() != metaObject()->className())
        return false;

    if(!reader.parameter(this, "MinimumNumberOfPossibilityToSelect", value) || value.isNull() || value.toInt() != minimumNumberOfPossibilityToSelect())
        return false;

    if(!reader.parameter(this, "MaximumNumberOfPossibilityThatCanBeSelected", value) || value.isNull() || value.toInt() != maximumNumberOfPossibilityThatCanBeSelected())
        return false;

    if(reader.parameterCount(this, "Possibility") != nPossibilitySaved())
        return false;

    QListIterator<CT_InStdModelPossibility*> it(m_possibilitiesGroup->getPossibilities());

    while(it.hasNext())
    {
        CT_InStdModelPossibility *possibility = it.next();

        const int pID = reader.parameter(this, "Possibility", value);

        if(!reader.parameterInfo(this, pID, "OutModelUniqueIndex", value) || value.toInt() != possibility->outModel()->uniqueIndex())
            return false;

        if(!reader.parameterInfo(this, pID, "IsSelected", value) || value.isNull())
            return false;

        possibility->setSelected(value.toBool());
    }

    reader.beginReadChildrens(this);

    const auto visitor = [&reader](const CT_InAbstractModel* model) -> bool{
        return const_cast<CT_InAbstractModel*>(model)->restoreSettings(reader);
    };

    visitInChildrensOrInModelOfPossibilities(visitor);

    reader.endReadChildrens(this);

    return true;
}

void CT_InAbstractModel::setOriginalModel(const CT_InAbstractModel* om)
{
    if(m_originalModel != nullptr)
        disconnect(m_originalModel, nullptr, this, nullptr);

    m_originalModel = const_cast<CT_InAbstractModel*>(om);

    if(m_originalModel != nullptr)
        connect(m_originalModel, SIGNAL(destroyed()), this, SLOT(originalModelDestroyed()), Qt::DirectConnection);
}

CT_InAbstractModel* CT_InAbstractModel::originalModel() const
{
    if(m_originalModel == nullptr)
        return const_cast<CT_InAbstractModel*>(this);

    return m_originalModel;
}

CT_InAbstractModel* CT_InAbstractModel::recursiveOriginalModel() const
{
    if(m_originalModel == nullptr)
        return const_cast<CT_InAbstractModel*>(this);

    return m_originalModel->recursiveOriginalModel();
}

bool CT_InAbstractModel::deleteLastSaveCycle()
{
    if(!m_saveCycles.isEmpty())
    {
        CT_InModelSaveCycle cycle = m_saveCycles.takeLast();

        while(!cycle.isEmpty())
        {
            CT_InStdModelPossibility *p = cycle.takeLast();
            m_possibilitiesGroup->removePossibility(p);
            delete p;
        }

        return true;
    }

    return false;
}

void CT_InAbstractModel::createAndAddAPossibility(const CT_OutAbstractModel& outModel)
{
    //DEBUG_MODEL_SEARCH("p : " << outModel.uniqueIndex() << " for " << this);

    if(m_saveCycles.isEmpty())
        m_saveCycles.append(CT_InModelSaveCycle());

    CT_InStdModelPossibility* possibility = createNewPossibility();
    possibility->setOutModel(outModel);

    m_saveCycles.last().append(possibility);
    m_possibilitiesGroup->addPossibility(possibility);
}

CT_InStdModelPossibility* CT_InAbstractModel::createNewPossibility() const
{
    return new CT_InStdModelPossibility();
}

bool CT_InAbstractModel::existModelInPossibilities(const CT_OutAbstractModel& outModel) const
{
    const IStepForModel* step = outModel.step();

    const auto visitor = [&](const CT_InStdModelPossibility* possibility) -> bool {
        const CT_OutAbstractModel* pOutModel = possibility->outModel();

        if((pOutModel == &outModel)
                && (pOutModel->step() == step))
            return false;

        return true;
    };

    return !m_possibilitiesGroup->visitPossibilities(visitor);
}

void CT_InAbstractModel::setMinimumNumberOfPossibilityToSelect(quint8 n)
{
    m_possibilitiesGroup->setMinimumNumberOfPossibilityThatMustBeSelected(n);
}

void CT_InAbstractModel::setMaximumNumberOfPossibilityThatCanBeSelected(int n)
{
    m_possibilitiesGroup->setMaximumNumberOfPossibilityThatCanBeSelected(n);
}

void CT_InAbstractModel::staticCopyPossibilitiesToModel(const CT_InAbstractModel* src, CT_InAbstractModel* dst)
{
    dst->clearPossibilitiesSaved();

    for(const CT_InModelSaveCycle& saveCycle : src->m_saveCycles)
    {
        CT_InModelSaveCycle newCycle;

        for(const CT_InStdModelPossibility* possibility : saveCycle)
        {
            CT_InStdModelPossibility* cpy = possibility->copy();

            dst->m_possibilitiesGroup->addPossibility(cpy);
            newCycle.append(cpy);
        }

        dst->m_saveCycles.append(newCycle);
    }
}

CT_InStdModelPossibilitySelectionGroup* CT_InAbstractModel::possibilitiesGroup() const
{
    return m_possibilitiesGroup;
}

bool CT_InAbstractModel::visitInChildrensOrInModelOfPossibilities(const InChildrenVisitor& visitor) const
{
    return visitInChildrens(visitor);
}

bool CT_InAbstractModel::recursiveVisitInChildrensOrInModelOfPossibilities(const CT_InAbstractModel::InChildrenVisitor& visitor) const
{
    const auto visitorAdapter = [&visitor](const CT_InAbstractModel* child) -> bool {
        if(!visitor(child))
            return false;

        return child->recursiveVisitInChildrensOrInModelOfPossibilities(visitor);
    };

    return visitInChildrensOrInModelOfPossibilities(visitorAdapter);
}

int CT_InAbstractModel::recursiveFindPossibilitiesInModel(const CT_OutAbstractModel& outModel)
{
    BEGIN_RECURSIVE_DEBUG

    if(!needOutputModel()) {
        END_RECURSIVE_DEBUG
        return 1;
    }

    DEBUG_MODEL_SEARCH("canBeComparedWith(" << &outModel << ") ?");

    // if this model cannot be compared with the current output model
    if(!canBeComparedWith(outModel)) {
        DEBUG_MODEL_SEARCH("NO");
        DEBUG_MODEL_SEARCH("Error return 0");
        END_RECURSIVE_DEBUG
        return 0;
    }

    DEBUG_MODEL_SEARCH("YES");

    // By default, if this model need output model and can be compared
    // with the output model, we set that is not ok (return code == 0)
    int finalOk = 0;
    bool recursiveOk = false;

    // if we can go down in childrens of the output model and must compare them with this input model
    // (used by CT_InZeroOrMoreGroupModel [*])
    if(mustCompareThisWithChildrenOfOutModel())
    {
        // we will be here in this situation :

        // input :
        // [*]

        // output :
        // Group (0)
        //  |--- Group (1)
        //  |       |--- Group (2)
        //  |
        //  |--- Group (3)

        // Per example we have checked that this input model ([*]) can be compared with the output model Group (0)
        // so now we will visit childrens of the ouput model Group (0).

        // This method will be recalled and we will be in the situation that the input model ([*]) was
        // compared to the output model Group (1).

        // As the Group (1) has a child this method will be once again recalled but the input model will be
        // compared to the output model Group (2). etc....

        DEBUG_MODEL_SEARCH("Go recursively down in childrens of " << &outModel);

        const auto visitor = [this, &recursiveOk](const CT_OutAbstractModel* child) -> bool {
            // go down and set recursiveOk to true if at least one hierarchy is compatible
            if(this->recursiveFindPossibilitiesInModel(*child) != 0)
                recursiveOk = true;

            return true;
        };

        outModel.visitOutChildrens(visitor);

        if(recursiveOk)
            finalOk = 2;
    }

    // if this output model cannot be a possibility for this input model
    // we return the last finalOk value
    if(!canBeAPossibility(outModel)) {
        DEBUG_MODEL_SEARCH(&outModel << " not added (" << (isOptionnal() ? "this = OPTIONNAL" : QString()) << ")");
        DEBUG_MODEL_SEARCH("return " << finalOk);
        END_RECURSIVE_DEBUG
        return finalOk;
    }

    QList<CT_InAbstractModel*> myInputChildrens;
    bool atLeastOneInputChildIsObligatory;

    // check if at least one child of this input model was obligatory and add all childrens in a collection to access it directly after
    const auto visitor = [&myInputChildrens, &atLeastOneInputChildIsObligatory] (const CT_InAbstractModel* child) -> bool {
        myInputChildrens.append(const_cast<CT_InAbstractModel*>(child));

        if(child->recursiveAtLeastOneChildrenOrThisIsObligatory())
            atLeastOneInputChildIsObligatory = true;

        return true;
    };

    this->visitInChildrens(visitor);

    DEBUG_MODEL_SEARCH_PRECISE_1("mustCompareChildrenWithThisModel ?");

    // if we must compare the current output model with childrens of this model
    // (used by CT_InZeroOrMoreGroupModel [*])
    if(mustCompareOutModelWithChildrenOfThisModel())
    {
        // we must go here in this situation :

        // input :
        // [*] (current input model)
        //   |---GroupX
        //          |----- InItem1

        // output :
        // RootGroup (current output model)
        //   |----- OutItem1

        // The child (GroupX) of the current input model [*] must be compared to the
        // current output model (Rootgroup)

        DEBUG_MODEL_SEARCH_PRECISE_1("YES");

        // we compare all childrens of this model with the current output model
        for(CT_InAbstractModel* myInputChild : myInputChildrens)
        {
            // we start a new save cycle
            myInputChild->startSaveCycle();

            // if the output model match with this child
            // and childrens of the output model match with childrens of this child
            // we set isOK to true
            const bool isOK = (myInputChild->recursiveFindPossibilitiesInModel(outModel) != 0);

            // finish the save cycle
            myInputChild->finishSaveCycle();

            if(!isOK)
                myInputChild->deleteLastSaveCycle();
            else
                finalOk = 1;
        }
    }
    else
    {
        DEBUG_MODEL_SEARCH_PRECISE_1("NO");
    }

    DEBUG_MODEL_SEARCH_PRECISE_1("mustCompareChildrenWithChildrenOfThisModel ?");

    // if we must compare childrens of this model with childrens of the output model
    if(mustCompareChildrenWithChildrenOfOutModel())
    {
        // we are here in this situation :

        // input :
        // Group or [*]
        //   |----- InItem1
        //   |----- InItem2

        // output :
        // ...
        //  |---Group (current model)
        //        |----- OutItem1

        // InItem1 and InItem2 must be compared with childrens of the current group (current output model)

        // In the case of the group of input models is NOT a [*] InItem1 and InItem2 must be found in childrens
        // of the current ouput model

        // In the case of the group of input models is a [*] at least only InItem1 or only InItem2 must be found in childrens
        // of the current ouput model

        DEBUG_MODEL_SEARCH_PRECISE_1("YES");

        bool allObligatoryChildrensHasFoundAtLeastOnePossibility = true;
        bool atLeastOneObligatoryChildHasFoundAtLeastOnePossibility = false;
        QList<bool> deleteLastSaveCycleList;

        // we compare all childrens of this input model with all childrens of the output model
        for(CT_InAbstractModel* myInputChild : myInputChildrens)
        {
            bool atLeastOnePossibilityFound = false;

            const auto visitor = [&myInputChild, &atLeastOnePossibilityFound](const CT_OutAbstractModel* child) -> bool {
                CT_OutAbstractModel* oam = const_cast<CT_OutAbstractModel*>(child);

                // if the output model match criteria for the children and recursively
                if(myInputChild->recursiveFindPossibilitiesInModel(*oam) != 0)
                    atLeastOnePossibilityFound = true;

                return true;
            };

            // we start a new save cycle
            myInputChild->startSaveCycle();

            outModel.visitOutChildrens(visitor);

            // finish the save cycle
            myInputChild->finishSaveCycle();

            if(myInputChild->recursiveAtLeastOneChildrenOrThisIsObligatory()) {
                if(!atLeastOnePossibilityFound) {
                    allObligatoryChildrensHasFoundAtLeastOnePossibility = false;
                    DEBUG_MODEL_SEARCH_PRECISE_1("Children (" << myInputChild << ") obligatory but no possibility found.");
                } else
                    atLeastOneObligatoryChildHasFoundAtLeastOnePossibility = true;
            }

            deleteLastSaveCycleList.append(atLeastOnePossibilityFound);
        }

        // if all obligatory childrens of the current input model has found
        // at least one possibility OR (if the current input model say that all obligatory childrens
        // must not be found in the same parent AND at least one of them has found a possibility)
        if(allObligatoryChildrensHasFoundAtLeastOnePossibility
                || (!mustAllObligatoryChildrensFoundAtLeastOnePossibilityInTheSameParent()
                    && atLeastOneObligatoryChildHasFoundAtLeastOnePossibility))
        {
            finalOk = 1;
        }
        else
        {
            const int size = deleteLastSaveCycleList.size();

            // we delete all saved cycle
            for(int index=0; index<size; ++index) {
                myInputChildrens.at(index)->deleteLastSaveCycle();
            }

            DEBUG_MODEL_SEARCH_PRECISE_1("all obligatory childrens must found at least one possibility but one of them has not found");
            DEBUG_MODEL_SEARCH_PRECISE_1("return " << finalOk);
            END_RECURSIVE_DEBUG
            return finalOk;
        }
    }
    else
    {
        DEBUG_MODEL_SEARCH_PRECISE_1("NO");
    }

    // if this model has no childrens or all was optionnal we consider that is ok
    if(!atLeastOneInputChildIsObligatory)
        finalOk = 1;

    // if all criteria are met
    if(finalOk == 1)
    {
        // if this model doesn't already exist in possibilities
        if(!existModelInPossibilities(outModel)) {
            DEBUG_MODEL_SEARCH("addToPossibility( " << &outModel << " )");
            // we create the possibility with this out model
            createAndAddAPossibility(outModel);
        }
    }
    else
    {
        DEBUG_MODEL_SEARCH(&outModel << " not added");
    }

    if(recursiveOk)
        finalOk = 2;

    DEBUG_MODEL_SEARCH("return " << finalOk);

    END_RECURSIVE_DEBUG

    return finalOk;
}

void CT_InAbstractModel::originalModelDestroyed()
{
    m_originalModel = nullptr;
}

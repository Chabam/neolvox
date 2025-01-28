#include "ct_modelselectionhelper.h"

#include "ct_model/inModel/abstract/ct_inabstractresultmodel.h"
#include "ct_model/inModel/tools/ct_instdresultmodelpossibility.h"
#include "ct_model/inModel/ct_inzeroormoregroupmodel.h"

#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_model/outModel/abstract/ct_outabstractmodel.h"

#include <QDebug>

QHash<CT_InAbstractResultModel*, CT_ModelSelectionHelper::CacheResult*> CT_ModelSelectionHelper::CACHED_GRAPHS;

CT_ModelSelectionHelper::CT_ModelSelectionHelper(const CT_InAbstractResultModel* inResultModel)
{
    Q_ASSERT(inResultModel != nullptr);
    if (inResultModel == nullptr) {qDebug() << "CT_ModelSelectionHelper::CT_ModelSelectionHelper" << ", " <<  "inResultModel == nullptr"; return;}

    m_rootInResultModel = const_cast<CT_InAbstractResultModel*>(inResultModel);

    QObject::connect(m_rootInResultModel, &CT_InAbstractResultModel::destroyed, [](QObject* resultModel) {
        CT_ModelSelectionHelper::CACHED_GRAPHS.remove(static_cast<CT_InAbstractResultModel*>(resultModel));
    });

    QObject::connect(m_rootInResultModel, &CT_InAbstractResultModel::startFindNewPossibilities, [](const CT_InAbstractModel* resultModel) {
        CT_ModelSelectionHelper::CACHED_GRAPHS.remove(static_cast<CT_InAbstractResultModel*>(const_cast<CT_InAbstractModel*>(resultModel)));
    });

    /*m_rootInResultModel->possibilitiesGroup()->visitPossibilities([this](const CT_InStdModelPossibility* p) -> bool {
        CT_InStdResultModelPossibility* poss = static_cast<CT_InStdResultModelPossibility*>(const_cast<CT_InStdModelPossibility*>(p));

        CT_InModelGraph* graph = new CT_InModelGraph();
        graph->constructStructure(m_rootInResultModel, poss);

        QList<CT_InModelGraph*> validGraphs;
        validGraphs << graph;

        this->recursiveConstructGraphs(validGraphs, poss->inResultModel());

        QMutableListIterator<CT_InModelGraph*> it(validGraphs);

        while(it.hasNext()) {
            CT_InModelGraph* g = it.next();

            if(g->isValid()) {
                m_validGraphsByRootPossibility.insert(poss, g);
                it.remove();
            }
        }

        qDeleteAll(validGraphs.begin(), validGraphs.end());
        return true;
    });*/

    //Q_ASSERT(isValid());
}

CT_ModelSelectionHelper::~CT_ModelSelectionHelper()
{
    //qDeleteAll(m_validGraphsByRootPossibility.begin(), m_validGraphsByRootPossibility.end());
}

/*bool CT_ModelSelectionHelper::isValid() const
{
    // if the number of possibilities for the result is less than the minimum that must be selected
    if(m_validGraphsByRootPossibility.uniqueKeys().size() < m_rootInResultModel->minimumNumberOfPossibilityToSelect())
        return false; // not valid !!

    return true;
}*/

bool CT_ModelSelectionHelper::recursiveCanSelectAllPossibilitiesByDefault() const
{
    CacheResult* cached = CACHED_GRAPHS.value(m_rootInResultModel, nullptr);

    if(cached != nullptr)
        return cached->m_canSelectAllPossibilitiesByDefault;

    cached = new CacheResult();
    cached->m_canSelectAllPossibilitiesByDefault = false;

    CACHED_GRAPHS.insert(m_rootInResultModel, cached);

    const int nResultSaved = m_rootInResultModel->nPossibilitySaved();

    if((nResultSaved > 1) || ((nResultSaved == 0) && m_rootInResultModel->needOutputModel()))
        return false;

    QHash<CT_OutAbstractModel*, int> m_modelChooseCount;

    const bool ok = m_rootInResultModel->recursiveVisitInChildrensOrInModelOfPossibilities([&m_modelChooseCount](const CT_InAbstractModel* child) -> bool {

        // If optionnal, return false because there is at least 2 possibilities : check or not optionnal
        if (dynamic_cast<const CT_InZeroOrMoreGroupModel*>(child) == nullptr && child->minimumNumberOfPossibilityToSelect() == 0)  {return false;}

        const int nSaved = child->nPossibilitySaved();

        // returns true if there is only one possibility that can be checked
        if(nSaved == 1) {
            // insert the number of times we found this out model for later process
            CT_OutAbstractModel* outModel = child->possibilitySavedAt(0)->outModel();
            m_modelChooseCount.insert(outModel, m_modelChooseCount.value(outModel, 0) + 1);
            return true;
        }

        // return true if there is no possibility and the model is optionnal
        return (nSaved == 0) && child->isOptionnal();

        // return false if there is more that one possibility or if there is no possibility saved
    });

    if(!ok)
        return false;

    // check if out models is only present one time in all possibilities
    for(const int& n : m_modelChooseCount.values()) {
        if(n > 1)
            return false; // otherwise return false
    }

    cached->m_canSelectAllPossibilitiesByDefault = true;
    CACHED_GRAPHS.insert(m_rootInResultModel, cached);

    return true;
    /*if(!isValid())
        return false;

    // we get only unique keys ! a key can exist multi times : it would like to say that there
    // are several choices for a possibility.
    QList<CT_InStdModelPossibility*> keys = m_validGraphsByRootPossibility.uniqueKeys();
    const int size = keys.size();

    const int maxNumberThatCanBeSelected = m_rootInResultModel->maximumNumberOfPossibilityThatCanBeSelected();

    // if there are more possibilities than the maximum number that can be selected
    if(size > maxNumberThatCanBeSelected)
        return false; // we can not select by default all possibilities, the user must do a choice

    const bool isOptionnal = m_rootInResultModel->isOptionnal();

    // if the result is optionnal and the result has at least one possibility
    if(isOptionnal
            && (size > 0))
        return false; // we can not select by default all possibilities, the user must do a choice

    // if there was several choice for one possibility
    if((size > 0) && (m_validGraphsByRootPossibility.keys().size() > size))
        return false; // we can not select by default all possibilities, the user must do a choice

    // if one model is optionnal in the structure we can not choose it by default, the user must do a choice
    if(size > 0)
        return !m_validGraphsByRootPossibility.value(keys.first(), nullptr)->hasOneModelOptional();

    return true;*/
}

bool CT_ModelSelectionHelper::recursiveSelectAllPossibilitiesByDefault()
{
    Q_ASSERT(recursiveCanSelectAllPossibilitiesByDefault() == true);
    if (!recursiveCanSelectAllPossibilitiesByDefault()) {qDebug() << "CT_ModelSelectionHelper::recursiveSelectAllPossibilitiesByDefault" << ", " <<  "!recursiveCanSelectAllPossibilitiesByDefault()"; return false;}

    if(m_rootInResultModel->nPossibilitySaved() > 0) {
        m_rootInResultModel->possibilitySavedAt(0)->setSelected(true);

        return m_rootInResultModel->recursiveVisitInChildrensOrInModelOfPossibilities([](const CT_InAbstractModel* child) -> bool {
            if(child->nPossibilitySaved() > 0)
                return child->possibilitySavedAt(0)->setSelected(true);

            return  true;
        });
    }

    return !m_rootInResultModel->needOutputModel();

    /*if(!recursiveCanSelectAllPossibilitiesByDefault())
        return false;

    return selectAtLeastOnePossibility();*/
}

/*bool CT_ModelSelectionHelper::canSelectAtLeastOnePossibility() const
{
    return !m_validGraphsByRootPossibility.isEmpty() || m_rootInResultModel->isOptionnal();
}

bool CT_ModelSelectionHelper::selectAtLeastOnePossibility()
{
    if(!canSelectAtLeastOnePossibility())
        return false;

    // we get only unique keys ! a key can exist multi times : it would like to say that there
    // are several choices for a possibility.
    QList<CT_InStdModelPossibility*> keys = m_validGraphsByRootPossibility.uniqueKeys();
    QListIterator<CT_InStdModelPossibility*> it(keys);

    // We select only one choice for a possibility
    while(it.hasNext())
        m_validGraphsByRootPossibility.value(it.next(), nullptr)->select();

    return true;
}

bool CT_ModelSelectionHelper::canSelectOneGraphForPossibilityOfRootModel(const CT_InStdModelPossibility *possibility, qint8 index) const
{
    if(index < 0)
        return !m_validGraphsByRootPossibility.values((CT_InStdModelPossibility*)possibility).isEmpty();

    return index < m_validGraphsByRootPossibility.values((CT_InStdModelPossibility*)possibility).size();
}
*/

bool CT_ModelSelectionHelper::selectOneGraphForPossibilityOfRootModel(const CT_InStdResultModelPossibility *possibility) const
{
    CT_InAbstractResultModel* inResultModel = possibility->inResultModel();

    inResultModel->recursiveVisitInChildrens([] (const CT_InAbstractModel* inModel)-> bool
    {
        if(inModel->nPossibilitySelected() > 0)
            return false;

        // if it is a leaf
        if(inModel->isEmpty())
        {
            const int nP = inModel->nPossibilitySaved();

            if(nP > 0)
            {
                const QString inDN = inModel->displayableName();
                const QString inDNlc = inDN.toLower();

                QMultiMap<double, CT_InStdModelPossibility*> bestPossibilities;

                for(int ip=0; ip<nP; ++ip)
                {
                    CT_InStdModelPossibility* p = inModel->possibilitySavedAt(ip);

                    if(inDN == p->outModel()->displayableName())
                        bestPossibilities.insert(1, p);
                    else if(inDNlc == p->outModel()->displayableName().toLower())
                        bestPossibilities.insert(0.8, p);
                    else
                        bestPossibilities.insert(0.5, p);
                }

                CT_InStdModelPossibility* bestPossibility = bestPossibilities.last();
                bestPossibility->setSelected(true);
                CT_OutAbstractModel* outModel = bestPossibility->outModel();

                // go up (recursively) to select parents
                CT_InAbstractModel* parentInModel = static_cast<CT_InAbstractModel*>(inModel->parentModel());

                int nP = 0;
                bool stop = false;

                // if the current model has a parent model and the parent model has at least one possibility saved
                while(!stop && (parentInModel != nullptr) && ((nP = parentInModel->nPossibilitySaved()) > 0))
                {
                    stop = true;

                    for(int i=0; i<nP; ++i)
                    {
                        CT_InStdModelPossibility* parentPossibility = parentInModel->possibilitySavedAt(i);

                        // if the out model used by the possibility if the parent of the current out model
                        if(parentPossibility->outModel() == outModel->parentModel()) {
                            // we select it (this will call the method "createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged"
                            // that will create a connection and this method will be called again until the out model has no parent)
                            parentPossibility->setSelected(true);
                            outModel = parentPossibility->outModel();
                            i = nP;
                            stop = false;
                        }
                    }

                    parentInModel = static_cast<CT_InAbstractModel*>(parentInModel->parentModel());
                }
            }
        }

        return true;
    });

    return true;
    /*if(!canSelectOneGraphForPossibilityOfRootModel(possibility, index))
        return false;

    CT_InModelGraph *graph = nullptr;

    if(index < 0)
        graph = getOneOfTheBestGraphForPossibilityOfRootModel(possibility);
    else
        graph = m_validGraphsByRootPossibility.values((CT_InStdModelPossibility*)possibility).at(index);

    Q_ASSERT(graph != nullptr);

    graph->select();

    return true;*/
}
/*
int CT_ModelSelectionHelper::nUniqueGraph() const
{
    return m_validGraphsByRootPossibility.uniqueKeys().size();
}

QList<CT_InStdModelPossibility *> CT_ModelSelectionHelper::uniqueGraphPossibilities() const
{
    return m_validGraphsByRootPossibility.uniqueKeys();
}

int CT_ModelSelectionHelper::nGraphForPossibilityOfRootModel(const CT_InStdModelPossibility *possibility) const
{
    return m_validGraphsByRootPossibility.values((CT_InStdModelPossibility*)possibility).size();
}

void CT_ModelSelectionHelper::debugPrintGraphs()
{
    int i = 0;

    QHashIterator<CT_InStdModelPossibility*, CT_InModelGraph*> it(m_validGraphsByRootPossibility);

    while(it.hasNext()) {
        qDebug() << "--------- Graph " << i << " -----------\r\n";
        it.next().value()->debugPrint();
        qDebug() << "---------------------------------------\r\n";
        ++i;
    }
}

void CT_ModelSelectionHelper::recursiveConstructGraphs(QList<CT_InModelGraph *> &graphsToModify, const CT_InAbstractModel *inModel)
{
    QList<CT_InModelGraph*> graphsOfCurrentLevel;

    QList<CT_InStdModelPossibility*> p = inModel->getPossibilitiesSaved();

    if(!p.isEmpty()) {
        QListIterator<CT_InStdModelPossibility*> it(p);

        int nCloned = 0;

        QList<CT_InModelGraph*> nOptionnalGraphFounded;

        const int nMaxToClone = 7;

        // maximum 6 possibilities (to be fastest) !
        while(it.hasNext() && (nCloned < nMaxToClone)) {
            CT_InStdModelPossibility *poss = it.next();

            QListIterator<CT_InModelGraph*> itG(graphsToModify);

            while(itG.hasNext()) {
                CT_InModelGraph *graph = itG.next();

                if(graph->canSetPossibilityForModel(poss, inModel)) {
                    CT_InModelGraph *newGraph = graph->clone();
                    newGraph->setPossibilityForModel(poss, inModel);
                    graphsOfCurrentLevel.append(newGraph);
                    ++nCloned;
                } else if(inModel->isOptionnal()) {
                    nOptionnalGraphFounded.append(graph);
                }
            }
        }

        while((nCloned < nMaxToClone) && !nOptionnalGraphFounded.isEmpty()) {
            CT_InModelGraph *newGraph = nOptionnalGraphFounded.takeFirst()->clone();
            graphsOfCurrentLevel.append(newGraph);
            ++nCloned;
        }

        qDeleteAll(graphsToModify.begin(), graphsToModify.end());
        graphsToModify = graphsOfCurrentLevel;
    }

    const auto visitor = [this, &graphsToModify](const CT_InAbstractModel* child) -> bool {
        this->recursiveConstructGraphs(graphsToModify, child);
        return true;
    };

    inModel->visitInChildrens(visitor);
}

CT_ModelSelectionHelper::CT_InModelGraph *CT_ModelSelectionHelper::getOneOfTheBestGraphForPossibilityOfRootModel(const CT_InStdModelPossibility *possibility) const
{
    CT_ModelSelectionHelper::CT_InModelGraph *bestGraph = nullptr;

    QList<CT_InModelGraph*> values = m_validGraphsByRootPossibility.values((CT_InStdModelPossibility*)possibility);

    if(!values.isEmpty()) {
        bestGraph = values.first();
        int bestScore = bestGraph->howManyPossibilitiesIsNotNullInStructure();

        QListIterator<CT_InModelGraph*> it(values);
        it.next();

        while(it.hasNext()) {
            CT_InModelGraph *g = it.next();
            int score = g->howManyPossibilitiesIsNotNullInStructure();

            if(score > bestScore) {
                bestScore = score;
                bestGraph = g;
            }
        }
    }

    return bestGraph;
}


CT_ModelSelectionHelper::CT_InModelGraph::CT_InModelGraph()
{
    m_rootNode = nullptr;
}

CT_ModelSelectionHelper::CT_InModelGraph::~CT_InModelGraph()
{
    delete m_rootNode;
}

void CT_ModelSelectionHelper::CT_InModelGraph::constructStructure(const CT_InAbstractResultModel *inResultModel, const CT_InStdResultModelPossibility *possibility)
{
    delete m_rootNode;
    m_rootNode = new CT_InModelGraphNode(inResultModel);
    m_rootNode->m_possibility = const_cast<CT_InStdResultModelPossibility*>(possibility);

    // root model
    staticRecursiveConstructStructure(possibility->inResultModel(), m_rootNode);
}

bool CT_ModelSelectionHelper::CT_InModelGraph::isValid() const
{
    return staticRecursiveIsValid(m_rootNode);
}

bool CT_ModelSelectionHelper::CT_InModelGraph::canSetPossibilityForModel(const CT_InStdModelPossibility *possibility, const CT_InAbstractModel *model) const
{
    CT_InModelGraphNode *node = staticRecursiveSearchNodeOfModel(m_rootNode, model);

    Q_ASSERT(node != nullptr);

    // if it is the root node or a children of the root node we always set the possibility.
    // explanation : 1. the root node (result) don't have a parent so the possibility can always be set.
    //               2. a child of the root node (a group of the result) has no parent too (the result model is not the parent) so the possibility can always be set.
    if((node == m_rootNode) || m_rootNode->m_childrensNode.contains(node))
        return true;

    // get the parent model of the out model of the possibility to set
    CT_OutAbstractModel *parentOutModel = (CT_OutAbstractModel*)possibility->outModel()->parentModel();

    // if it was nullptr -> it is a result model
    // if the parent was nullptr -> it is a root group
    if((parentOutModel == nullptr) || (parentOutModel->parentModel() == nullptr))
        return true;

    CT_InAbstractModel *parentModel = static_cast<CT_InAbstractModel*>(model->parentModel());

    Q_ASSERT(parentModel != nullptr);

    CT_InModelGraphNode *parentNode = staticRecursiveSearchNodeOfModel(m_rootNode, parentModel);

    Q_ASSERT(parentNode != nullptr);

    if((parentNode->m_possibility == nullptr) && !parentNode->m_model->needOutputModel())
        return true;

    if(parentNode->m_model->isObligatory() && (parentNode->m_possibility != nullptr)) {
        Q_ASSERT(parentNode->m_possibility != nullptr);

        if(parentNode->m_possibility->outModel() == possibility->outModel()->parentModel()) {

            CT_InModelGraphNode *nodeP = staticRecursiveSearchNodeOfOutModel(m_rootNode, possibility->outModel());

            // the out model of the possibility must not be already used by another model !
            return (nodeP == nullptr);
        }
    }

    return false;
}

bool CT_ModelSelectionHelper::CT_InModelGraph::setPossibilityForModel(const CT_InStdModelPossibility *possibility, const CT_InAbstractModel *model)
{
    if(!canSetPossibilityForModel(possibility, model))
        return false;

    CT_InModelGraphNode *node = staticRecursiveSearchNodeOfModel(m_rootNode, model);

    node->m_possibility = (CT_InStdModelPossibility*)possibility;

    return true;
}

CT_InStdModelPossibility* CT_ModelSelectionHelper::CT_InModelGraph::possibilityForModel(const CT_InAbstractModel *model) const
{
    CT_InModelGraphNode *node = staticRecursiveSearchNodeOfModel(m_rootNode, model);
    Q_ASSERT(node != nullptr);

    return node->m_possibility;
}

bool CT_ModelSelectionHelper::CT_InModelGraph::hasOneModelOptional() const
{
    return staticRecursiveIsOneModelOptional(m_rootNode);
}

void CT_ModelSelectionHelper::CT_InModelGraph::select()
{
    staticRecursiveSelectPossibility(m_rootNode, true);
}

void CT_ModelSelectionHelper::CT_InModelGraph::clearSelection()
{
    staticRecursiveSelectPossibility(m_rootNode, false);
}

CT_ModelSelectionHelper::CT_InModelGraph *CT_ModelSelectionHelper::CT_InModelGraph::clone() const
{
    CT_ModelSelectionHelper::CT_InModelGraph *graph = new CT_ModelSelectionHelper::CT_InModelGraph();

    if(m_rootNode == nullptr)
        return graph;

    graph->m_rootNode = m_rootNode->cloneAlone();

    staticRecursiveClone(m_rootNode, graph->m_rootNode);

    return graph;
}

int CT_ModelSelectionHelper::CT_InModelGraph::howManyPossibilitiesIsNotNullInStructure() const
{
    return staticRecursiveCountPossibilityNotnullptr(m_rootNode);
}

void CT_ModelSelectionHelper::CT_InModelGraph::debugPrint()
{
    staticRecursiveDebugPrintGraph(m_rootNode);
}

void CT_ModelSelectionHelper::CT_InModelGraph::staticRecursiveConstructStructure(const CT_InAbstractModel *inModel, CT_ModelSelectionHelper::CT_InModelGraphNode *parentNode)
{
    CT_InModelGraphNode *node = new CT_InModelGraphNode(inModel);
    parentNode->m_childrensNode.append(node);

    const auto visitor = [&node](const CT_InAbstractModel* child) -> bool {
        staticRecursiveConstructStructure(child, node);
        return true;
    };

    inModel->visitInChildrens(visitor);
}

bool CT_ModelSelectionHelper::CT_InModelGraph::staticRecursiveIsValid(const CT_ModelSelectionHelper::CT_InModelGraphNode *node)
{
    if(node == nullptr)
        return true;

    Q_ASSERT(node->m_model != nullptr);

    if((node->m_possibility == nullptr) && (node->m_model->isOptionnal()))
        return true;
    else if(node->m_possibility == nullptr)
        return false;

    QListIterator<CT_InModelGraphNode*> it(node->m_childrensNode);

    while(it.hasNext()) {
        if(!staticRecursiveIsValid(it.next()))
            return false;
    }

    return true;
}

CT_ModelSelectionHelper::CT_InModelGraphNode* CT_ModelSelectionHelper::CT_InModelGraph::staticRecursiveSearchNodeOfModel(const CT_ModelSelectionHelper::CT_InModelGraphNode *node, const CT_InAbstractModel *model)
{
    if(node == nullptr)
        return nullptr;

    if(node->m_model == model)
        return (CT_ModelSelectionHelper::CT_InModelGraphNode*)node;

    QListIterator<CT_InModelGraphNode*> it(node->m_childrensNode);

    while(it.hasNext()) {
        CT_InModelGraphNode *foundedNode;

        if((foundedNode = staticRecursiveSearchNodeOfModel(it.next(), model)) != nullptr)
            return foundedNode;
    }

    return nullptr;
}

CT_ModelSelectionHelper::CT_InModelGraphNode *CT_ModelSelectionHelper::CT_InModelGraph::staticRecursiveSearchNodeOfOutModel(const CT_ModelSelectionHelper::CT_InModelGraphNode *node, const CT_OutAbstractModel *outModel)
{
    if(node == nullptr)
        return nullptr;

    if((node->m_possibility != nullptr) && (node->m_possibility->outModel() == outModel))
        return (CT_ModelSelectionHelper::CT_InModelGraphNode*)node;

    QListIterator<CT_InModelGraphNode*> it(node->m_childrensNode);

    while(it.hasNext()) {
        CT_InModelGraphNode *foundedNode;

        if((foundedNode = staticRecursiveSearchNodeOfOutModel(it.next(), outModel)) != nullptr)
            return foundedNode;
    }

    return nullptr;
}

void CT_ModelSelectionHelper::CT_InModelGraph::staticRecursiveSelectPossibility(CT_ModelSelectionHelper::CT_InModelGraphNode *node, bool select)
{
    if(node == nullptr)
        return;

    if(node->m_possibility != nullptr)
        node->m_possibility->setSelected(select);

    QListIterator<CT_InModelGraphNode*> it(node->m_childrensNode);

    while(it.hasNext())
        staticRecursiveSelectPossibility(it.next(), select);
}

void CT_ModelSelectionHelper::CT_InModelGraph::staticRecursiveClone(CT_ModelSelectionHelper::CT_InModelGraphNode *myNode, CT_ModelSelectionHelper::CT_InModelGraphNode *cloneNode)
{
    QListIterator<CT_InModelGraphNode*> it(myNode->m_childrensNode);

    while(it.hasNext()) {
        CT_InModelGraphNode *myCNode = it.next();
        CT_InModelGraphNode *cloneCNode = myCNode->cloneAlone();

        cloneNode->m_childrensNode.append(cloneCNode);

        staticRecursiveClone(myCNode, cloneCNode);
    }
}

bool CT_ModelSelectionHelper::CT_InModelGraph::staticRecursiveIsOneModelOptional(CT_ModelSelectionHelper::CT_InModelGraphNode *node)
{
    if(node == nullptr)
        return false;

    Q_ASSERT(node->m_model != nullptr);

    if(node->m_model->isOptionnal())
        return true;

    QListIterator<CT_InModelGraphNode*> it(node->m_childrensNode);

    while(it.hasNext()) {
        if(staticRecursiveIsOneModelOptional(it.next()))
            return true;
    }

    return false;
}

int CT_ModelSelectionHelper::CT_InModelGraph::staticRecursiveCountPossibilityNotnullptr(CT_ModelSelectionHelper::CT_InModelGraphNode *node)
{
    if(node == nullptr)
        return 0;

    int count = 0;

    if(node->m_possibility != nullptr)
        count = 1;

    QListIterator<CT_InModelGraphNode*> it(node->m_childrensNode);

    while(it.hasNext())
        count += staticRecursiveCountPossibilityNotnullptr(it.next());

    return count;
}

void CT_ModelSelectionHelper::CT_InModelGraph::staticRecursiveDebugPrintGraph(CT_ModelSelectionHelper::CT_InModelGraphNode *node, QString preString)
{
    QString preStringNext = preString;

    if(node->m_possibility != nullptr) {
        qDebug() << preString << node->m_possibility->outModel()->displayableName() << "(" << node->m_model->displayableName() << ")";
        preStringNext += "\t";
    }

    QListIterator<CT_InModelGraphNode*> it(node->m_childrensNode);

    while(it.hasNext())
        staticRecursiveDebugPrintGraph(it.next(), preStringNext);
}
*/

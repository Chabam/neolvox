#ifndef CT_MODELSELECTIONHELPER_H
#define CT_MODELSELECTIONHELPER_H

#include "ct_model/inModel/abstract/ct_inabstractmodel.h"

class CT_InAbstractResultModel;
class CT_InStdResultModelPossibility;

/**
 * @brief This class can be used to select default possibility in a tree of models. The root model
 *        is a CT_InAbstractResultModel (a model that represent a result).
 *
 */
class CTLIBMODELS_EXPORT CT_ModelSelectionHelper
{
public:
    CT_ModelSelectionHelper(const CT_InAbstractResultModel* inResultModel);
    ~CT_ModelSelectionHelper();

    /**
     * @brief Returns true if it was at least one possibility. If return false it was an error in development or in use !
     */
    //bool isValid() const;

    // -- COMPLETE SELECTION -- //

    /**
     * @brief Returns true if all possibilities can be selected by default. If the user must do a choice we return false. Per
     *        example if the result model has multiple possibilities and his number is greater than the maximum number
     *        of possibility that can be selected, the method return false because the user must choose which possibility he want to use.
     */
    bool recursiveCanSelectAllPossibilitiesByDefault() const;

    /**
     * @brief Select one possibility for all in models in the structure.
     * @return false if the user must choose among several possibilities.
     */
    bool recursiveSelectAllPossibilitiesByDefault();

    // -- PARTIAL SELECTION -- //

    /**
     * @brief Returns true if we can select at least one possibility for each model in the structure. False if it is not
     *        possible. This may happened if there was not enough possibility for models of a given level.
     */
    //bool canSelectAtLeastOnePossibility() const;

    /**
     * @brief Select one possibility for each model in the structure.
     * @return false if it is not possible. This may happened if there was not enough possibility for models of a given level.
     */
    //bool selectAtLeastOnePossibility();

    /**
     * @brief Returns true if at least one graph that contains the possibility is present and can be selected.
     * @param possibility : one of possibility of the iResultModel
     * @param index : the index of the graph to select (if -1 search the best by default)
     */
    //bool canSelectOneGraphForPossibilityOfRootModel(const CT_InStdModelPossibility *possibility, qint8 index = -1) const;

    /**
     * @brief Select the first graph that contains the possibility.
     * @param possibility : one of possibility of the iResultModel
     * @return false if there was no graph present for this possibility
     */
    bool selectOneGraphForPossibilityOfRootModel(const CT_InStdResultModelPossibility* possibility) const;

    // -- OTHER -- //

    /**
     * @brief Returns the number of unique graph
     */
    //int nUniqueGraph() const;

    /**
     * @brief Returns the list of possibilities that represent a key for a graph
     */
    //QList<CT_InStdModelPossibility*> uniqueGraphPossibilities() const;

    /**
     * @brief Returns the number of graph (number of choice) for a possibility
     */
    //int nGraphForPossibilityOfRootModel(const CT_InStdModelPossibility *possibility) const;

    //void debugPrintGraphs();

private:
    /**
     * @brief A CT_InModelGraphNode is a node in a CT_InModelGraph (graph of CT_InAbstractModel).
     */
//    class CT_InModelGraphNode
//    {
//    public:

//        CT_InModelGraphNode(const CT_InAbstractModel *model) : m_model((CT_InAbstractModel*)model), m_possibility(nullptr) {}
//        ~CT_InModelGraphNode()
//        {
//            qDeleteAll(m_childrensNode.begin(), m_childrensNode.end());
//        }

//        CT_InAbstractModel              *m_model;
//        CT_InStdModelPossibility        *m_possibility;

//        QList<CT_InModelGraphNode*>     m_childrensNode;

//        CT_InModelGraphNode* cloneAlone() const {
//            CT_InModelGraphNode* cl = new CT_InModelGraphNode(m_model);
//            cl->m_possibility = m_possibility;

//            return cl;
//        }
//    };

    /**
     * @brief A CT_InModelGraph is the structure of CT_InAbstractModel. A developper can test and assign a possibility to a CT_InAbstractModel in the structure. If the
     *        graph is valid (all models has one possibility) the developper can use it to test/select possibilities by default.
     */
//    class CT_InModelGraph
//    {
//    public:
//        CT_InModelGraph();
//        ~CT_InModelGraph();

//        /**
//         * @brief (Re)Construct the structure of the graph. All models don't have a possibility when you construct the structure. You must assign it with the method "setPossibilityForModel"
//         * @param rootInModel : the root model to use to construct the structure
//         */
//        //void constructStructure(const CT_InAbstractResultModel *inResultModel, const CT_InStdResultModelPossibility *possibility);

//        /**
//         * @brief Returns true if the graph is valid, all models has one possibility or no possibility if the model is optionnal.
//         */
//        //bool isValid() const;

//        /**
//         * @brief Returns true if the possibility can be set for this model. It will test if the out model of the parent possibility in the
//         *        structure is the same parent of the out model of this possibility. It will also test if the possibility is not used by another
//         *        model in the structure (only one model can use this possibility).
//         * @param possibility : possibility to test
//         * @param model : the model to use
//         * @return always true for the root model. false if the parent possibility in the structure is not set or if is not a parent of this possibility.
//         */
//        //bool canSetPossibilityForModel(const CT_InStdModelPossibility *possibility, const CT_InAbstractModel *model) const;

//        /**
//         * @brief Set the possibility for this model.
//         * @param possibility : possibility to set
//         * @param model : the model to use
//         * @return Show method "canSetPossibilityForModel" for more information.
//         */
//        //bool setPossibilityForModel(const CT_InStdModelPossibility *possibility, const CT_InAbstractModel *model);

//        /**
//         * @brief Returns the possibility used for the model.
//         * @param model : model to search
//         * @return possibility founded or nullptr if it was not set or not founded.
//         */
//        //CT_InStdModelPossibility* possibilityForModel(const CT_InAbstractModel *model) const;

//        /**
//         * @brief Returns true if one model in the structure is optionnal.
//         */
//        //bool hasOneModelOptional() const;

//        /**
//         * @brief Select possibilities in the graph
//         */
//        //void select();

//        /**
//         * @brief Unselect all possibilities in the graph
//         */
//        //void clearSelection();

//        /**
//         * @brief Clone this graph with model and possibilities
//         */
//        //CT_InModelGraph* clone() const;

//        /**
//         * @brief Count and returns the number of possibilities that is not nullptr in the structure
//         */
//        //int howManyPossibilitiesIsNotNullInStructure() const;

//        //void debugPrint();

//    private:
//        CT_InModelGraphNode *m_rootNode;

//        /**
//         * @brief Recursively construct the structure of the graph.
//         */
//        //static void staticRecursiveConstructStructure(const CT_InAbstractModel *inModel, CT_InModelGraphNode *parentNode);

//        /**
//         * @brief Returns true if the graph is valid, all models has one possibility.
//         */
//        //static bool staticRecursiveIsValid(const CT_InModelGraphNode *node);

//        /**
//         * @brief Search a node in the structure that match with the inModel.
//         */
//        //static CT_InModelGraphNode* staticRecursiveSearchNodeOfModel(const CT_InModelGraphNode *node, const CT_InAbstractModel *model);

//        /**
//         * @brief Search a node in the structure that match with the outModel.
//         */
//        //static CT_InModelGraphNode* staticRecursiveSearchNodeOfOutModel(const CT_InModelGraphNode *node, const CT_OutAbstractModel *outModel);

//        /**
//         * @brief Recursively select possibility of the node
//         */
//        //static void staticRecursiveSelectPossibility(CT_InModelGraphNode *node, bool select);

//        /**
//         * @brief Recursively clone the structure (model and possibility)
//         */
//        //static void staticRecursiveClone(CT_InModelGraphNode *myNode, CT_InModelGraphNode *cloneNode);

//        /**
//         * @brief Recursively check if a model is optionnal
//         */
//        //static bool staticRecursiveIsOneModelOptional(CT_InModelGraphNode *node);

//        /**
//         * @brief Recursively count possibilities that is not nullptr
//         */
//        //static int staticRecursiveCountPossibilityNotnullptr(CT_InModelGraphNode *node);

//        //static void staticRecursiveDebugPrintGraph(CT_InModelGraphNode *node, QString preString = "");
//    };

    struct CacheResult {
        bool m_canSelectAllPossibilitiesByDefault;
    };

    static QHash<CT_InAbstractResultModel*, CacheResult*> CACHED_GRAPHS;

    CT_InAbstractResultModel*       m_rootInResultModel;
    //QMultiHash<CT_InStdModelPossibility*, CT_InModelGraph*> m_validGraphsByRootPossibility;

    /**
     * @brief Recursively construct all graphs that was valid
     * @param graphsToModify : the list of final valid graphs (must contains on first call one graph with an empty structure)
     * @param inModel : the model to use to fill the graph with his possibilities
     */
    //void recursiveConstructGraphs(QList<CT_InModelGraph*> &graphsToModify, const CT_InAbstractModel *inModel);

    /**
     * @brief In all graphs for the possibility passed in parameter, returns the graph that have the maximum number of possibility != nullptr
     */
    //CT_InModelGraph* getOneOfTheBestGraphForPossibilityOfRootModel(const CT_InStdModelPossibility *possibility) const;
};

#endif // CT_MODELSELECTIONHELPER_H

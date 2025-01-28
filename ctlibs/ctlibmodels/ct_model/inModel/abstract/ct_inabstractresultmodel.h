#ifndef CT_INABSTRACTRESULTMODEL_H
#define CT_INABSTRACTRESULTMODEL_H

#include "ct_model/inModel/abstract/ct_inabstractmodel.h"

class CT_OutAbstractResultModel;

/**
 * @brief This is the base class for an input model that represent a result. Childrens of this model
 *        will never have possibilities because a result model can have multiple possibilities. So in
 *        each possibility of this model that was of type "CT_InStdResultModelPossibility" you can
 *        retreive a input result model that was a copy of this one but with childrens that has
 *        theirs possibilities.
 */
class CTLIBMODELS_EXPORT CT_InAbstractResultModel : public CT_InAbstractModel
{
    Q_OBJECT
    typedef CT_InAbstractModel SuperClass;

public:
    typedef std::function<bool(const IResultForModel*)> RealResultVisitor;

    /**
     * @brief Construct a result model
     * @param displayableName : a displayable name
     */
    CT_InAbstractResultModel(const QString& displayableName);

    /**
     * @brief Copy constructor (doesn't copy possibilities)
     */
    CT_InAbstractResultModel(const CT_InAbstractResultModel& other);

    /**
     * @brief Set to true if you want to force recursivity on all new input result model created.
     */
    static bool FORCE_RECURSIVITY;

    /**
     * @brief Set to true if you want that this model research recusively results in all step in the tree (go up in hierarchy) or false
     *        if you want just research in results of the step passed in parameter of the method "searchResultsInStep..."
     * @warning if you change this parameter you must recall method "searchResultsInStep..."
     */
    void setRecursive(bool r);

    /**
     * @brief Returns true if this model search results in all step in the tree (go up in tree) or false if search just in results of the step
     *        passed in parameter of the method "searchResultsInStep..."
     */
    bool isRecursive() const;

    /**
     * @brief Redefined to make a copy of this result model before do the search and add it to the possibility if all has been found. Emit the signal
     *        "startFindNewPossibilities()" at the beginning of this method.
     */
    bool recursiveFindAllPossibilitiesInModel(const CT_OutAbstractModel& rootModel, bool mustClearPossibilitiesSaved = true) override;

    /**
     * @brief Redefined to use the recursivity parameter
     */
    bool recursiveHasTheMinimumNumberOfSelectedPossibilityRequired(QStringList* errors = nullptr) const final;

    /**
     * @brief Redefined because this model is a result and create a tree for each possibility. The possibility
     *        has a root input result model.
     */
    bool visitInChildrensOrInModelOfPossibilities(const InChildrenVisitor& visitor) const override;

    /**
     * @brief Visit all real results that match with selected possibilities of this result model.
     */
    bool visitRealResultsThatMatchWithSelectedPossibilities(const RealResultVisitor& visitor) const;

    /**
     * @brief Visit all real results that match with the specified possibility.
     */
    bool visitRealResultsThatMatchWithThisPossibility(const CT_InStdModelPossibility* possibility,
                                                      const RealResultVisitor& visitor) const;

    /**
     * @brief Recursively search in this result model the child that was a copy of the specified original model
     * @param originalModel : the original model to compare with the "originalModel" returned by child in this result model
     * @return The copied model if founded or the original model if it was in this result model. nullptr if not founded.
     */
    CT_InAbstractModel* recursiveSearchTheModelThatWasACopiedModelFromThisOriginalModel(const CT_InAbstractModel* originalModel) const;

protected:
    /**
     * @brief Set if this model must be recursive or not without use the FORCE_RECURSIVITY value.
     */
    void setRecursiveWithoutUseForceRecursivity(bool e);

    /**
     * @brief Set if the model passed in parameter must be recursive or not without use the
     *        FORCE_RECURSIVITY value. Used by subclass.
     */
    static void staticSetRecursiveWithoutUseForceRecursivity(CT_InAbstractResultModel *model, bool e);

    /**
     * @brief Create a new possibility. Redefined to return a CT_InStdResulModelPossibility instead of a CT_InStdModelPossibility.
     */
    CT_InStdModelPossibility* createNewPossibility() const override;

private:
    /**
     * @brief True if we must research recusively results in all step in the tree (go up in hierarchy), false otherwise
     */
    bool                                m_recursive;

    /**
     * @brief A temporary object that was created and set in method "recursiveFindAllPossibilitiesInModel" or set to
     *        a possibility in method "createNewPossibility"
     */
    mutable CT_InAbstractResultModel*   m_currentInResultModelCopy;

    /**
     * @brief Visit the real result that use the output result model. Do the search only in the step returned by the output model.
     * @return Returns the visitor value or true if no result was founded.
     */
    bool visitRealResultForModel(const CT_OutAbstractResultModel* model, const RealResultVisitor& visitor) const;
};

#endif // CT_INABSTRACTRESULTMODEL_H

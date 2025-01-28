#ifndef CT_INABSTRACTMODEL_H
#define CT_INABSTRACTMODEL_H

#include "ct_model/abstract/ct_abstractmodel.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"

class CT_OutAbstractModel;

/**
 * @brief This is the base class for INPUT model.
 *
 *        An input model can search compatible output models in a tree of
 *        output models. It will create a collection of possibility for each
 *        output model compatible.
 *
 *        An input model can be optionnal or obligatory (by default) and can
 *        search zero, one or multiple possibility.
 */
class CTLIBMODELS_EXPORT CT_InAbstractModel : public CT_AbstractModel
{
    Q_OBJECT
    using SuperClass = CT_AbstractModel;

public:
    using InChildrenVisitor = std::function<bool(const CT_InAbstractModel*)>;
    using PossibilityVisitor = std::function<bool(const CT_InAbstractModel* model, const CT_InStdModelPossibility* possibility)>;
    using Possibilities = CT_InStdModelPossibilitySelectionGroup::Possibilities;

    /**
     * @brief Construct an input model
     * @param displayableName : a displayable name
     */
    CT_InAbstractModel(const QString &displayableName);

    /**
     * @brief Copy constructor (doesn't copy possibilities)
     */
    CT_InAbstractModel(const CT_InAbstractModel& other);

    ~CT_InAbstractModel();

    /**
     * @brief Set the minimum number of possibility that must be selected
     * @param n : 0 if you want that this model was optionnal, at least 1 if this model must be obligatory.
     * @warning No check is made to define if minimum is lower than maximum, you are responsible to do this check.
     */
    void setMinimumNumberOfPossibilityToSelect(quint8 n);

    /**
     * @brief Set the maximum number of possibility that can be selected. This will not set the maximum number of possibility
     *        that the model can find but only the maximum number of possibility that can be selected ! There is no method
     *        to define the maximum number of possibility that can be found.
     * @param n : -1 if you want that it was no limit, otherwise at least the minimum number of possibility to select.
     * @warning No check is made to define if maximum is greather or equal to minimum, you are responsible to do this check.
     */
    void setMaximumNumberOfPossibilityThatCanBeSelected(int n);

    /**
     * @brief Returns the minimum number of possibility that must be selected
     */
    quint8 minimumNumberOfPossibilityToSelect() const;

    /**
     * @brief Returns the maximum number of possibility that can be selected. -1 if it was no limit.
     */
    int maximumNumberOfPossibilityThatCanBeSelected() const;

    /**
     * @brief Returns true if this model is optionnal (minimum number of possibility is 0 or this model doesn't need output model)
     */
    bool isOptionnal() const;

    /**
     * @brief Returns true if this model is obligatory (minimum number of possibility is > 0 and this model need output model)
     */
    bool isObligatory() const;

    /**
     * @brief Returns true if this model or at least one children (recursively) is obligatory (minimum number of possibility is > 0 and this model need output model)
     */
    bool recursiveAtLeastOneChildrenOrThisIsObligatory() const;

    /**
     * @brief A model doesn't need a output model if the minimum and the maximum of possibility is set to 0.
     */
    bool needOutputModel() const;

    /**
     * @brief Called to visit childrens (input models) of this model.
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_InAbstractModel and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool visitInChildrens(const InChildrenVisitor& visitor) const;

    /**
     * @brief Call it to visit the childrens of this model and recursively childrens of children's, etc....
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_InAbstractModel and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool recursiveVisitInChildrens(const InChildrenVisitor& visitor) const;

    /**
     * @brief Called to visit childrens (input models) of this model or childrens (input models) of possibilities if this model
     *        use a possibility like a tree and the possibility has a root input model.
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_InAbstractModel and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    virtual bool visitInChildrensOrInModelOfPossibilities(const InChildrenVisitor& visitor) const;

    /**
     * @brief Called to visit recursively childrens (input models) of this model or childrens (input models) of possibilities if this model
     *        use a possibility like a tree and the possibility has a root input model.
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_InAbstractModel and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool recursiveVisitInChildrensOrInModelOfPossibilities(const InChildrenVisitor& visitor) const;

    /**
     * @brief Recursively visit possibilities of this model and children's possibilities
     * @param visitor : a lambda expression or a function member that receive a pointer to a
     *                  CT_AbstractModel and a pointer to a CT_InStdModelPossibility and return a bool (true to continue the visit, false to stop the visit)
     */
    bool recursiveVisitPossibilities(const PossibilityVisitor& visitor) const;

    /**
     * @brief Visit all possibilities of this model
     * @param visitor : a lambda expression or a function member that receive a pointer to a
     *                  CT_AbstractModel and a pointer to a CT_InStdModelPossibility and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool visitPossibilities(const PossibilityVisitor& visitor) const;

    /**
     * @brief Visit all selected possibilities of this model
     * @param visitor : a lambda expression or a function member that receive a pointer to a
     *                  CT_AbstractModel and a pointer to a CT_InStdModelPossibility and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool visitSelectedPossibilities(const PossibilityVisitor& visitor) const;

    /**
     * @brief Returns an object that you can use to iterate over saved possibilities
     */
    Possibilities savedPossibilities() const;

    /**
     * @brief Returns an object that you can use to iterate over selected possibilities
     */
    Possibilities selectedPossibilities() const;

    /**
     * @brief Start a new save cycle that can be undoable
     */
    void startSaveCycle();

    /**
     * @brief End the current save cycle
     */
    void finishSaveCycle();

    /**
     * @brief Returns the number of save cycle created
     */
    int nSaveCycle() const;

    /**
     * @brief Find all possibilities in the out model and its children compatible with this model and his childrens. Emit the signal
     *        "startFindNewPossibilities()" at the beginning of this method.
     * @param rootModel : the output model where to begin the search
     * @return true if all models has at least one possibility if it must have one. Otherwise return false. If the output model
     *         cannot be compared with this model the method returns automatically false.
     */
    virtual bool recursiveFindAllPossibilitiesInModel(const CT_OutAbstractModel& rootModel, bool mustClearPossibilitiesSaved = true);

    /**
     * @brief Verify if this model have at least one possibility selected.
     * @return True if this model have at least one possibility selected.
     */
    bool isAtLeastOnePossibilitySelected() const;

    /**
     * @brief Verify if this model and childrens too have at least one possibility selected.
     * @return True if this model and childrens too have at least one possibility selected.
     */
    bool recursiveIsAtLeastOnePossibilitySelected() const;

    /**
     * @brief Verify if the minimum number of possibility found in this model is greather or equal than "minimumNumberOfPossibilityToSelect()".
     * @param errors : you can pass a pointer to a string list that will contains all errors if the method return false, otherwise the list will be empty
     * @return true is the test pass.
     */
    bool hasTheMinimumNumberOfPossibilityRequired(QStringList* errors = nullptr) const;

    /**
     * @brief Verify if the minimum number of possibility found in this model and in childrens is greather or equal than "minimumNumberOfPossibilityToSelect()".
     * @param errors : you can pass a pointer to a string list that will contains all errors if the method return false, otherwise the list will be empty
     * @return true is the test pass.
     */
    bool recursiveHasTheMinimumNumberOfPossibilityRequired(QStringList* errors = nullptr) const;

    /**
     * @brief Verify if the minimum number of possibility selected in this model is greather or equal than "minimumNumberOfPossibilityToSelect()".
     * @param errors : you can pass a pointer to a string list that will contains all errors if the method return false, otherwise the list will be empty
     * @return true is the test pass.
     */
    bool hasTheMinimumNumberOfSelectedPossibilityRequired(QStringList* errors = nullptr) const;

    /**
     * @brief Verify if the minimum number of possibility selected in this model and in childrens is greather or equal than "minimumNumberOfPossibilityToSelect()".
     * @param errors : you can pass a pointer to a string list that will contains all errors if the method return false, otherwise the list will be empty
     * @return true is the test pass.
     */
    virtual bool recursiveHasTheMinimumNumberOfSelectedPossibilityRequired(QStringList* errors = nullptr) const;

    /**
     * @brief Returns the group that contains possibilities
     * @warning Not intended for direct use by plugin developper
     */
    CT_InStdModelPossibilitySelectionGroup* possibilitiesGroup() const;

    /**
     * @brief Returns the number of possibility saved by this model
     */
    int nPossibilitySaved() const;

    /**
     * @brief Returns the number of possibility selected
     */
    int nPossibilitySelected() const;

    /**
     * @brief Returns the possibility saved at the specified index
     * @param index : the index of the possibility (must be between [0; nPossibilitySaved()])
     * @return The possibility at specified index.
     * @warning No control is made to verify the validity of the index.
     */
    CT_InStdModelPossibility* possibilitySavedAt(const int& index) const;

    /**
     * @brief Returns the possibility saved at the specified index
     * @param index : the index of the possibility (must be between [0; nPossibilitySaved()])
     * @return The possibility at specified index.
     * @warning No control is made to verify the validity of the index.
     */
    CT_InStdModelPossibility* possibilitySelectedAt(const int& index) const;

    /**
     * @brief Returns all possibilities saved by this model
     */
    QList<CT_InStdModelPossibility*> getPossibilitiesSaved() const;

    /**
     * @brief Returns all possibilities saved by this model and selected by the user or automatically
     */
    QList<CT_InStdModelPossibility*> getPossibilitiesSelected() const;

    /**
     * @brief Clear all possibitilies saved by this model
     */
    void clearPossibilitiesSaved();

    /**
     * @brief Clear all possibitilies saved by this model and its children
     */
    void recursiveClearPossibilitiesSaved();

    /**
     * @brief Save all necessary elements to be able to restore it with the method "restoreSettings"
     */
    virtual void saveSettings(SettingsWriterInterface& writer) const;

    /**
     * @brief Restore the state
     */
    virtual bool restoreSettings(SettingsReaderInterface& reader);

    /**
     * @brief Returns a copy of this model and its children.
     * @param withPossibilities : true if you want to copy possibilities in the copy of the model
     */
    virtual CT_InAbstractModel* copy(bool withPossibilities) const = 0;

    /**
     * @brief Return true if this model can be compared with the specified output model (check by dynamic_cast per example)
     */
    virtual bool canBeComparedWith(const CT_OutAbstractModel& model) const = 0;

    /**
     * @brief Must return true if the speicified output model can be a possibility. By default returns the result of "canBeComparedWith"
     */
    virtual bool canBeAPossibility(const CT_OutAbstractModel& model) const { return canBeComparedWith(model); }

    /**
     * @brief Returns the original model of this model if it was a copy otherwise returns this
     */
    CT_InAbstractModel* originalModel() const;

    /**
     * @brief Returns the original model (recursively) of this model if it was a copy otherwise returns this
     */
    CT_InAbstractModel* recursiveOriginalModel() const;

protected:
    /**
     * @brief A save cycle contains a collection of possibility founded
     */
    class CT_InModelSaveCycle : public QList<CT_InStdModelPossibility*>
    {
    public:
        CT_InModelSaveCycle() : QList<CT_InStdModelPossibility*>() {}
    };

    /**
     * @brief Set the original model of this model if it was a copy
     */
    void setOriginalModel(const CT_InAbstractModel *om);

    /**
     * @brief Find all possibilities in the out model for this model and its children
     * @param outModel : the output model where begin the search
     * @return 1 if all model had at least one possibility if it must have one. 2 if it's ok recursively. Otherwise return 0. If the out model
     *         cannot be compared with this model the method return automatically 0.
     */
    int recursiveFindPossibilitiesInModel(const CT_OutAbstractModel& outModel);

    /**
     * @brief Must return true if the current output model must be compare to childrens of the output model. Per example
     *        if your input model can be anywhere in the tree structure.
     *
     *        This method is called after the method "canBeComparedWith(....)" if it returns true
     */
    virtual bool mustCompareThisWithChildrenOfOutModel() const { return false; }

    /**
     * @brief Must return true if the current output model must be compared with childrens of this input model.
     *
     *        This method is called after methods "canBeComparedWith(....)" "canAddPossibilityForThisModel(...)" if
     *        it returns true.
     */
    virtual bool mustCompareOutModelWithChildrenOfThisModel() const { return false; }

    /**
     * @brief Must return true if we can continue the research with children of the
     *        current OUTPUT model.
     *
     *        This method is called after the method "canBeComparedWith(....)" if it returns true
     */
    virtual bool mustCompareChildrenWithChildrenOfOutModel() const { return true; }

    /**
     * @brief Must return true if all obligatory childrens of this input model must found at least one possibility in
     *        the same parent
     */
    virtual bool mustAllObligatoryChildrensFoundAtLeastOnePossibilityInTheSameParent() const { return true; }

    /**
     * @brief Create and add a possibility to the collection of possibilities and to the current save cycle
     */
    void createAndAddAPossibility(const CT_OutAbstractModel& outModel);

    /**
     * @brief Create a new possibility.
     *        You can redefine this method if you want to create a special type of possibility that inherit from CT_InStdModelPossibility
     */
    virtual CT_InStdModelPossibility* createNewPossibility() const;

    /**
     * @brief Copy all possibilities from one model to another
     */
    static void staticCopyPossibilitiesToModel(const CT_InAbstractModel *src, CT_InAbstractModel *dst);

private:
    /**
     * @brief The collection of save cycle
     */
    QList< CT_InModelSaveCycle >                m_saveCycles;

    /**
     * @brief The group that contains all possibility founded
     */
    CT_InStdModelPossibilitySelectionGroup*     m_possibilitiesGroup;

    /**
     * @brief The original model used to create this result model
     */
    CT_InAbstractModel*                         m_originalModel;

    /**
     * @brief Remove from the list (m_saveCycles) the last save cycle
     *        and all possibilities of this cycle from the group (m_possibilitiesGroup)
     * @return false if the list is empty
     */
    bool deleteLastSaveCycle();

    /**
     * @brief Return true if the model already exist in the list of possibilities
     */
    bool existModelInPossibilities(const CT_OutAbstractModel& outModel) const;

private slots:
    /**
     * @brief Called when the original model was destroyed
     */
    void originalModelDestroyed();

signals:
    /**
     * @brief Emitted when this model start to find new possibilities
     */
    void startFindNewPossibilities(const CT_InAbstractModel* model);
};

#endif // CT_INABSTRACTMODEL_H

#ifndef CT_INMANAGER_H
#define CT_INMANAGER_H

#include "ctlibstep_global.h"

#include "settingsinterfaces.h"
#include "ct_model/inModel/manager/ct_inmodelstructuremanager.h"
#include "ct_model/inModel/tools/ct_inresultmodelconfigurationmanager.h"

class CT_VirtualAbstractStep;

/**
 * @brief This class manage inputs (model and results)
 */
class CTLIBSTEP_EXPORT CT_InManager
{
public:
    CT_InManager();
    ~CT_InManager();

    /**
     * @brief Returns the input models manager
     */
    CT_InModelStructureManager* inModelStructureManager() const;

    /**
     * @brief Call all necessary methods to declare input models.
     * @return If the step has at least one children this method returns false. True otherwise.
     */
    bool createInputModels(CT_VirtualAbstractStep& step);

    /**
     * @brief Returns true if it was at least one model in the model manager
     */
    bool needInputs() const;

    /**
     * @brief Start from this step and go up recursively to find necessary input
     *        in output of this step or his parents. All possibility of input models
     *        will not be modified when you call this method because a copy is made before
     *        do the process.
     * @param step : the step where to start the search
     * @return false if the step is nullptr and this step need inputs OR if all models doesn't find necessary
     *         input. Otherwise if input has found what they want this method returns true.
     */
    bool wereAllInputsFoundInOutputsOfThisStepOrRecursively(const CT_VirtualAbstractStep* step) const;

    /**
     * @brief Start from this step and go up recursively to find necessary input
     *        in output of this step or his parents. All possibility of input models
     *        will be modified when you call this method !
     * @param step : the step where to start the search
     * @return false if the step is nullptr and this step need inputs OR if all models doesn't find necessary
     *         input. Otherwise if input has found what they want this method returns true and
     *         possibilities can be used to see which out models has been saved.
     */
    bool findInputsInOutputsOfThisStepOrRecursively(const CT_VirtualAbstractStep* step);

    enum ConfigureReturn {
        NoModification,
        HasModification,
        Error
    };

    /**
     * @brief Show the input elements configuration dialog. This dialog allow the user to select elements that must be used in "compute" method
     *        for searched elements. The dialog is displayed in read only mode if the step has childrens or if we can not configure something.
     * @param forceReadOnly : true to force to show the input configuration dialog in read only mode
     * @return Returns false if the user cancel the configuration.
     */
    ConfigureReturn configureInputs(const CT_VirtualAbstractStep* step, bool forceReadOnly = false);

    /**
     * @brief Returns true if all models (recursively) has the minimum number of selection necessary. In other words
     *        this method check if all models are configured correctly.
     */
    bool hasAllModelsTheMinimumNumberOfSelectedPossibilityRequired() const;

    /**
     * @brief A input result model visitor is a function that returns false it the visit must be stopped, otherwise it
     *        must return true. The function will receive a pointer to a result model.
     */
    using InResultModelVisitor = std::function<bool(const CT_InAbstractResultModel*)>;

    /**
     * @brief Visit all input result model
     * @param visitor : a lambda expression per example that return a bool (true if must continue the visit) and receive a const pointer to a CT_InAbstractResultModel
     * @return True if no result has been visited, otherwise the returned value of the visitor
     */
    bool visitResultModels(const InResultModelVisitor& visitor) const;

    /**
     * @brief Returns the number of input result models
     */
    int nResultModel() const;

    /**
     * @brief Returns the number of input results
     */
    int nResults() const;

    /**
     * @brief Call the method "setBusy" of all input results in this manager
     * @param busy : the parameter to pass to the method "setBusy" of all input results
     */
    void setResultsBusy(bool busy);

    /**
     * @brief Must be called to inform that it will be deleted
     * @warning Call it in GUI thread
     */
    void aboutToBeDeleted();

    /**
     * @brief Redefined to save the item type
     */
    void saveSettings(SettingsWriterInterface& writer) const;

    /**
     * @brief Redefined to restore the item type
     */
    bool restoreSettings(SettingsReaderInterface& reader, QUrl pathHelp);

    /**
     * @brief Export screen capture of IN model configuration dialog
     */
    bool exportViewForINModelConfig(QString exportPath, QStringList &exportedFiles);

private:

    CT_InModelStructureManager              m_inModelsStructureManager;
    CT_InResultModelConfigurationManager*   m_inModelConfigurationManager;
    QString                                 m_stepName;

    /**
     * @brief Start from this step and go up recursively to find necessary input
     *        in output of this step or his parents. Use result model that was inside the
     *        specified collection.
     * @param step : the step where to start the search
     * @param begin : the begin iterator of a collection that contains pointer to CT_InAbstractResultModel
     * @param end : the end iterator of a collection that contains pointer to CT_InAbstractResultModel
     * @return false if the step is nullptr and this step need inputs OR if all models doesn't find necessary
     *         input. Otherwise if input has found what they want this method returns true and
     *         possibilities can be used to see which out models has been saved.
     */
    bool findInputsInOutputsOfThisStepOrRecursivelyWithSpecifiedResultModels(const CT_VirtualAbstractStep* step,
                                                                            QVector<CT_InAbstractResultModel*>::iterator begin,
                                                                            QVector<CT_InAbstractResultModel*>::iterator end) const;
};

#endif // CT_INMANAGER_H

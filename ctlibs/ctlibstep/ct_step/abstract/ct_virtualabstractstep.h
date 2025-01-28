/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/

#ifndef CT_VIRTUALABSTRACTSTEP_H
#define CT_VIRTUALABSTRACTSTEP_H

#include "ctlibstep_global.h"

#include "settingsinterfaces.h"

#include "ct_step/interfacesforstep.h"
#include "ct_step/tools/input/ct_inmanager.h"
#include "ct_step/tools/output/ct_outmanager.h"
#include "ct_step/tools/ct_uniqueindexgenerator.h"
#include "ct_step/tools/ct_logmanageradapterforstep.h"
#include "ct_step/view/ct_stepconfigurabledialog.h"

#include "ct_result/abstract/ct_abstractresult.h"

#include "ct_model/inModel/manager/ct_inmodelstructuremanager.h"
#include "ct_model/outModel/manager/ct_outmodelstructuremanager.h"

#include "ct_model/outModel/abstract/ct_outabstractresultmodel.h"
#include "ct_model/inModel/abstract/ct_inabstractresultmodel.h"
#include "ct_model/inModel/abstract/ct_inabstractitemmodel.h"
#include "ct_model/outModel/abstract/ct_outabstractitemmodel.h"
#include "ct_model/inModel/abstract/ct_inabstractitemattributemodel.h"
#include "ct_model/outModel/abstract/ct_outabstractitemattributemodel.h"


#include <QObject>
#include <QDateTime>
#include <QElapsedTimer>

#include <QDebug>

class CT_AbstractStepPlugin;

using CT_StepInModelStructureManager = CT_InModelStructureManager;
using CT_StepOutModelStructureManager = CT_OutModelStructureManager;

/**
 * @brief A step is an object that declare inputs that she need, can be configured and declare
 *        outputs that she will produce. A step can be added after another step if inputs that she need
 *        was found in outputs of the first parent step or recursively (depend of the configuration) in second,
 *        third, etc... parent steps.
 *
 *        A step can be debuggable if she has declared that she is (method "isDebuggable()"). When she
 *        can be debugged the processing will stop in some specified break point, defined by the
 *        developper of the step, and some debug informations can be displayed in documents if the developper
 *        has written code to do it.
 *
 *        A step can be manual if she has declared that she is (method "isManual()"). This mode
 *        provides the ability to edit elements manually in documents via actions (see ActionInterface).
 *
 *        ###### A step can be modifiable if she has declared that she is (method "isModifiable()"). This mode
 *        provides the ability to edit the step. Not coded yet !!! ######
 */
class CTLIBSTEP_EXPORT CT_VirtualAbstractStep : public QObject, public IStepForModel
{
    Q_OBJECT

public:
    using UniqueIndexGeneratorPtr = CT_UniqueIndexGenerator*;

    CT_VirtualAbstractStep();
    ~CT_VirtualAbstractStep();

    IStepToolForModel* stepToolForModel() const;

    /************ INFORMATION ************/

    /**
     * @brief Return the unique id of the step
     * @warning Not intended for direct use by plugin developper
     */
    int uniqueID() const;

    /**
     * @brief Returns a tooltip to display it in a gui per example. By default returns the result
     *        of the method "description()".
     *        Redefine this method if you want to return your own tooltip.
     */
    virtual QString toolTip() const;

    /**
     * @brief Returns a name that is not translatable. By default return the result of
     *        call to "metaObject()->className()".
     *
     *        Inherit this method if you want to return your own name. This name must be
     *        unique to compare it if a script is loaded per example. If you want to show a name
     *        in gui interface you can overload the method "displayableName()".
     */
    virtual QString name() const;

    /**
     * @brief Returns an extended name that is not translatable. By default return the result of the
     *        method "name()" + "(" + uniqueID() + ")".
     *
     *        Inherit this method if you want to return your own extended name.
     */
    virtual QString extendedName() const;

    /**
     * @brief Returns a displayable name that is translatable. By default returns the result
     *        of the method "name()".
     *
     *        Inherit this method if you want to return your own displayable name that will be used
     *        in gui if the "custom name" has not be changed by the user.
     */
    virtual QString displayableName() const;

    /**
     * @brief Returns an extended name that is translatable. By default return the result of the
     *        method "displayableName()" + "(" + uniqueID() + ")".
     *
     *        Inherit this method if you want to return your own displayable extended name.
     */
    virtual QString displayableExtendedName() const;

    /**
     * @brief Return the custom name, that is translatable, if it was not empty otherwise return the result
     *        of the method "displayableExtendedName()".
     */
    QString displayableCustomName() const;

    /**
     * @brief Call it to set a custom displayable name
     */
    void setDisplayableCustomName(const QString& customName);

    /**
     * @brief Returns a translatable description to inform what does this step. By default return a string
     *        to inform that there is no description for this step.
     *
     *        Inherit this method if you want to return your own description.
     */
    virtual QString description() const;

    /**
     * @brief Returns a translatable detailled description to inform what does this step. By default return a string
     *        to inform that there is no detailled description for this step.
     *
     *        Inherit this method if you want to return your own description.
     */
    virtual QString detailledDescription() const;

    /**
     * @brief Returns an url to a web site for a description of the step and more. By default return the plugin's url.
     *
     *        Inherit this method if you want to return your own URL.
     */
    virtual QString URL() const;

    /**
     * @brief Returns citations for step (in RIS format) as a String list
     *
     *        Inherit this method to give citations
     *
     */
    virtual QStringList getStepRISCitations() const;

    /**
     * @brief Returns parameters description for step
     *
     *        By default parameters' help is generated automatically. So in general case this method should not be redefined
     *        Inherit this method if you want to provide a ad-hoc parameters description
     *
     */
    virtual QString parametersDescription(bool addDescriptions = true);

    /**
     * @brief Returns description of inputs results for step
     *
     *        By default input results help is generated automatically. So in general case this method should not be redefined
     *        Inherit this method if you want to provide a ad-hoc input results description
     *
     */
    virtual QString inputDescription() const;

    /**
     * @brief Returns description of output results for step
     *
     *        By default output results help is generated automatically. So in general case this method should not be redefined
     *        Inherit this method if you want to provide a ad-hoc output results description
     *
     */
    virtual QString outputDescription() const;

    /**
     * @brief Returns details on how this step works
     *
     *        Inherit this method if you want to provide details on step working
     *
     */
    virtual QString detailsDescription() const;


    /**
     * @brief Set the plugin of this step
     */
    void setPlugin(const IPluginForStep* plugin);

    /**
     * @brief Return the plugin
     * @warning Not intended for direct use by plugin developper
     */
    IPluginForStep* plugin() const;

    /**
     * @brief Returns the plugin statically cast in the class that you want
     */
    template<class T = CT_AbstractStepPlugin>
    T* pluginStaticCastT() const { return static_cast<T*>(plugin()); }

    /**
     * @brief Set the parent step of this step
     * @param parent : the parent step
     */
    void setParentStep(const CT_VirtualAbstractStep* parent);

    /**
     * @brief Call it to set this step was a prototype (a prototype is a class instancied only to create new instance of the same type)
     */
    void setIsAPrototype();

    /**
     * @brief Returns true if this object is a prototype (only instancied to create new instance of the same type), false otherwise
     */
    bool isAPrototype() const;

    /************ CREATION ************/

    /**
     * @brief Create and returns a new empty instance of this step. Only the default constructor must be called and the object must be returned.
     */
    virtual CT_VirtualAbstractStep* createNewInstance() const = 0;

    /************ INITIALIZATION ************/

    /**
     * @brief This method must be called after the constructor (if this step is a prototype or not) ! By default
     *        it does nothing.
     *
     *        Inherit this method if you want to initialize something.
     */
    virtual void init();

    /**
     * @brief Called from GUI before step was added to the steps tree and after method "init()"
     *
     *        This method call :
     *          - fillPreInputConfigurationDialog()     -> redefine this method to fill the pre input configuration dialog
     *          - preInputConfigure()                   -> redefine this method if you want to show a personnalized pre input configuration dialog
     */
    bool showPreInputConfigurationDialog();

    /**
     * @brief Called from GUI before step was added to the steps tree and after method "showPreInputConfigurationDialog()"
     *
     *        This method call :
     *          - configureInputResult()        -> redefine this method if you want to initialize something after models has been created and after the pre configuration
     *                                             ATTENTION : if you inherit this method call the method of the superclass first !
     *
     *        and create all IN turns and IN models
     */
    bool showInputResultConfigurationDialog();

    /**
     * @brief Called from GUI before step was added to the steps tree and after method "showInputResultConfigurationDialog()"
     *
     *        This method call :
     *          - fillPostInputConfigurationDialog()    -> redefine this method to fill the post input configuration dialog
     *          - postInputConfigure()                  -> redefine this method if you want to show a personnalized post input configuration dialog
     */
    bool showPostConfigurationDialog();

    /**
     * @brief Returns true if the configuration can be finalized. You can call this method before the method "showPostConfigurationDialog".
     * @param why : The reason why we can not finalize the configuration if this method returns false. Empty otherwise.
     * @return true if you can call "finalizeConfiguration", false otherwise.
     */
    bool canFinalizeConfiguration(CT_OutManager::CreateOutputModelsErrorType* errorType = nullptr, QString* why = nullptr) const;

    /**
     * @brief Called from GUI before step was added to the steps tree and after method "showPostConfigurationDialog()" or after "restoreOthersSettings()"
     *        and create all output models
     */
    bool finalizeConfiguration();

    /**
     * @brief Return true if settings of the step has been modified. If true the
     *        processing of the step must be relaunched.
     */
    virtual bool isSettingsModified() const;

    /************ STRUCTURE (TREE) CREATION/MODIFICATION/VISIT ************/

    /**
     * @brief Call this method to know if a step can be added after another step. Check if input result
     *        that this step need was found in the output of the specified step or recursively in other
     *        step if it does. (Use models, not real result !)
     * @param step : the parent step or nullptr if this step must be added as a root step.
     * @return True if this step can be added after the specified step
     */
    bool acceptAddAfterThisStep(const CT_VirtualAbstractStep* step);

    /**
     * @brief Add a step to the collection of childrens of this step. The
     *        life in memory of the step will now be managed by this step.
     *        The parent step of the child will be set to this step.
     * @return False if the step can not be added in the child collection of this
     *         step because some input was not found. The step is not deleted from
     *         memory in this case !
     *
     *         To be sure that this method returns true you must call the
     *         method "step->acceptAddAfterThisStep(parentStep)" and if she returns
     *         true you can call this method to add a step in the collection.
     */
    bool appendStep(CT_VirtualAbstractStep* step);

    /**
     * @brief Remove a step from the collection of childrens of this step and
     *        delete it from the memory.
     * @returns True if it was in the collection of child and has been removed, false otherwise.
     */
    bool removeStep(CT_VirtualAbstractStep* step);

    /**
     * @brief A children visitor is a function that returns false it the visit must be stopped, otherwise it
     *        must return true. The function will receive a pointer to a parent step and to a child step.
     * @param parentStep : the parent step of the child step
     * @param childStep : a child step
     */
    using ChildVisitor = std::function<bool(const CT_VirtualAbstractStep*, const CT_VirtualAbstractStep*)>;

    /**
     * @brief Visit childrens of this step
     * @return True if no child has been visited, otherwise the returned value of the visitor
     */
    bool visitChildrens(const ChildVisitor& visitor) const;

    /**
     * @brief Recursively visit childrens of this step
     * @return True if no child has been visited, otherwise the returned value of the visitor
     */
    bool recursiveVisitChildrens(const ChildVisitor& visitor) const;

    /**
     * @brief Returns true if this step has childrens
     */
    bool hasChildrens() const;

    /**
     * @brief Return the parent (nullptr if this step has no parent)
     */
    CT_VirtualAbstractStep* parentStep() const;

    /**
     * @brief Return the root step (step that has no parent) of the hierarchy. If this step is the root
     *        step it will return this.
     */
    CT_VirtualAbstractStep* rootStep() const;

    /************ INPUTS ************/

    /**
     * @brief Return true if this step must have at least on input result
     */
    bool needInputResults() const;

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
    bool visitInResultModels(const InResultModelVisitor& visitor) const;

    /************ OUTPUTS ************/
    /**
     * @brief A input result model visitor is a function that returns false it the visit must be stopped, otherwise it
     *        must return true. The function will receive a pointer to a result model.
     */
    using OutResultModelVisitor = std::function<bool(const CT_OutAbstractResultModel*)>;

    /**
     * @brief Visit all output result model
     * @param visitor : a lambda expression per example that return a bool (true if must continue the visit) and receive a const pointer to a CT_OutAbstractResultModel
     * @return True if no results has been visited, otherwise the returned value of the visitor
     */
    bool visitOutResultModels(const OutResultModelVisitor& visitor) const;

    /**
     * @brief A input result model visitor is a function that returns false it the visit must be stopped, otherwise it
     *        must return true. The function will receive a pointer to a result model.
     */
    using OutResultVisitor = std::function<bool(const CT_AbstractResult*)>;

    /**
     * @brief Visit all output result
     * @param visitor : a lambda expression per example that return a bool (true if must continue the visit) and receive a const pointer to a CT_AbstractResult
     * @return True if no results has been visited, otherwise the returned value of the visitor
     */
    bool visitOutResults(const OutResultVisitor& visitor) const;

    /**
     * @brief Returns the output result at specified index
     */
    const CT_AbstractResult* outResultAt(const int& index) const;

    /**
     * @brief Returns the number of output result
     */
    int nOutResult() const;

    /**
     * @brief Clear output results from memory and from the collection.
     */
    void clearOutResult();

    /**
     * @brief Only clear what is inside output results from memory. The collection of results is not modified. Called from core or gui.
     */
    void clearOutResultFromMemory();

    /************ PROCESS ************/

    /**
     * @brief Call it to launch the processing. It will clear current output results, launch the processing
     *        and produce new output results.

     *        This method :
     *          - Clear output results of this step and childrens too
     *          - Retrieve input results and set it busy
     *          - Call the method "compute()"
     */
    void execute();

    /**
     * @brief Stop the processing of the step. This method only works if the developper of a step
     *        call the method "isStopped()" in loops of his processing.
     */
    void stop();

    /**
     * @brief Return true if this step is running
     */
    bool isRunning() const;

    /**
     * @brief Return true if this step is stopped
     */
    bool isStopped() const;

    /**
     * @brief Return the number of milliseconds that have elapsed since the method "compute" has been called
     */
    int executeTime() const;

    /**
     * @brief Return the current progress value of the method "compute". This method only works
     *        if the developper of a step use the method "setProgress()" to change the progress.
     */
    int progressValue() const;

    /**
     * @brief Return the current error code. 0 if no error.
     * @warning Not intended for direct use by plugin developper
     */
    int errorCode() const;

    /**
     * @brief Return the error message for the error code passed in parameter
     */
    virtual QString errorMessage(int errorCode) const;

    /**
     * @brief Returns true if this step want that the core restart this step. By default return true if setting is modified or if
     *        result was cleared from memory.
     */
    virtual bool mustBeRestarted() const;

    /**
     * @brief Returns the step where we must restart the process or nullptr if we must continue. By default return nullptr.
     */
    virtual CT_VirtualAbstractStep* restartComputeFromStep() const;

    /************ SAVE/RESTORE ************/

    /**
     * @brief This method is called when the core will save settings of pre configuration dialog
     * @warning Don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual void savePreSettings(SettingsWriterInterface& writer) const;

    /**
     * @brief This method is called when the core will save settings of input model
     * @warning Don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual void saveInputSettings(SettingsWriterInterface& writer) const;

    /**
     * @brief This method is called when the core will save settings of post configuration dialog
     * @warning Don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual void savePostSettings(SettingsWriterInterface& writer) const;

    /**
     * @brief This method is called when the core will save settings of others elements in this step like readers, etc...
     * @warning Don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual void saveOthersSettings(SettingsWriterInterface& writer) const;

    /**
     * @brief This method is called when the core will restore settings of pre configuration dialog
     * @warning Don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual bool restorePreSettings(SettingsReaderInterface& reader);

    /**
     * @brief This method is called when the core will restore settings of input model
     * @warning Don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual bool restoreInputSettings(SettingsReaderInterface& reader);

    /**
     * @brief This method is called when the core will restore settings of post configuration dialog
     * @warning Don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual bool restorePostSettings(SettingsReaderInterface& reader);

    /**
     * @brief This method is called when the core will restore settings of others elements in this step like readers, etc...
     * @warning Don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual bool restoreOthersSettings(SettingsReaderInterface& reader);

    /************ DESTRUCTION ************/

    /**
     * @brief Must be called from GUI (in the gui thread) before step was deleted
     *
     *        This method delete all configuration dialogs. It is the only place to delete your dialog because this method
     *        was called in the GUI thread.
     *
     * @warning If you inherit this method don't forget to call the method of the SuperClass !
     */
    virtual void aboutToBeDeleted();

    /************ OTHERS ************/

    /**
     * @brief Called from GUI to set a special context when the step go in debug mode or manual mode.
     *
     *  3 possibilities :
     *
     *  - If debug mode is active this method is called before the step compute. After it was
     *    finished the method is called with context = nullptr.
     *
     *  - Every time the step requires the manual mode this method is called if the GuiContextInterface
     *    returned by the method "getGuiContext()" is different from the current context. After the step
     *    quit the manual mode this method is called with context = nullptr.
     *
     *  - If debug mode is active this method is called before the step compute. Every time the step
     *    requires the manual mode this method is called if the GuiContextInterface returned by the
     *    method "getGuiContext()" is different from the current context. After she finished
     *    calculating, the method is called with context = nullptr.
     */
    void setGuiContext(const GuiContextInterface *context);

    /**
     * @brief Return the gui context in manual mode or debug mode. See method "setGuiContext" for more information.
     */
    GuiContextInterface* guiContext() const;

    /************ DEBUG MODE ************/

    /**
     * @brief Return true if this step is debuggable.
     *        The developper of a step must use the method "setDebuggable(true)" to define it debuggable.
     */
    bool isDebuggable() const;

    /**
     * @brief Set the debug mode on/off.
     * @returns True if this step is debuggable and not running !
     */
    bool setDebugModeOn(bool enable);

    /**
     * @brief Returns true if the debug mode is on.
     */
    bool isDebugModeOn() const;

    /**
     * @brief Call it to go to the next break point if the signal "waitForAckInDebugMode()" has been emitted.
     * @param nBreakPointToJump : the number of break point to jump. 1 by default to go to the next break point.
     */
    void ackDebugMode(int nBreakPointToJump = 1);

    /************ MANUAL MODE ************/

    /**
     * @brief Return true if this step use the manual mode.
     *        The developper of a step must use the method "setManual(true)" to define it manual.
     */
    bool isManual() const;

    /**
     * @brief Return true if this step is in manual mode.
     */
    bool isInManualMode() const;

    /**
     * @brief Call it after the step request a manual mode.
     */
    void ackManualMode();

    /**
     * @brief Call it to quit the manual mode.
     */
    void quitManualMode();

    /************ MODIFIABLE MODE ************/

    /**
     * @brief Return true if this step is modifiable.
     *        The developper of a step must use the method "setModifiable(true)" to define it modifiable.
     */
    bool isModifiable() const;

    /**
     * @brief Call it to launch the step in modification mode. The processing must have be launched first
     *        with a call to the method "execute()".
     *
     *       This method :
     *          - Clear output results of childrens only (not of this step)
     *          - Retrieve input results and set it busy
     *          - Call the method "modify()"
     */
    void executeModify();

protected:

    /************ CONFIGURATION ************/

    /**
     * @brief Call this method when at least one of your settings of your step (typically after a configuration dialog is accepted) is modified.
     *
     * A signal is emitted when this variable has changed. The method "isSettingsModified()" is used to know if the compute of the
     * step must be restarted.
     *
     * @param modified : true if you want to inform that settings is modified
     */
    virtual void setSettingsModified(bool modified);

    /************ PRE INPUT CONFIGURATION ************/

    /**
     * @brief Redefine this method if you want to fill the pre input configuration dialog.
     *
     * The pre input configuration dialog is displayed after the method "configureInputs" is called and before the method "declareOutputModels" will be called. If you want to fill
     * a standard pre input configuration dialog use the variable in argument. Settings of this dialog will be automaticallysaved and restored. If you want to use your
     * own dialog you must redefine method "preInputConfigure()" to display your dialog, "savePreSettings(...)" and "restorePreSettings(...)" to save/restore pre settings of your step.
     */
    virtual void fillPreInputConfigurationDialog(CT_StepConfigurableDialog* preInputConfigDialog) { Q_UNUSED(preInputConfigDialog)}

    /**
     * @brief Show the pre configuration dialog.
     *
     * If you want to show your own configuration dialog your must overload this method and show your dialog when this method is called. Don't forget
     * to call the method "setSettingsModified(true)" if your settings is modified (if user accept your dialog).
     *
     * @return Returns false if the user cancel the configuration.
     */
    virtual bool preInputConfigure();

    /**
     * @brief Called after the pre input dialog has been displayed and user accept the change OR after pre settings has
     *        been restored
     */
    virtual void finalizePreSettings() {}

    /************ INPUTS DECLARATION ************/

    /**
     * @brief Called when the step must declare all input models. The manager must be used in
     *        conjunction with CT_HandleInXXX to add input that the step need.
     */
    virtual void declareInputModels(CT_StepInModelStructureManager& manager) = 0;

    /************ INPUTS CONFIGURATION (USER SELECT INPUTS) ************/

    /**
     * @brief Show the input elements configuration dialog. This dialog allow the user to select elements that must be used in "compute" method
     *        for searched elements. The dialog is displayed in read only mode if the step has childrens or if we can not configure something.
     * @param forceReadOnly : true to force to show the input configuration dialog in read only mode
     * @return Returns false if the user cancel the configuration.
     */
    virtual bool configureInputs(bool forceReadOnly = false);

    /************ POST INPUT CONFIGURATION ************/

    /**
     * @brief Redefine this method if you want to fill the post input configuration dialog.
     *
     * The post configuration dialog is displayed after the method "configureInputs" is called and before the method "declareOutputModels" will be called. If you want to fill
     * a standard post input configuration dialog use the variable in argument. Settings of this dialog will be automaticallysaved and restored. If you want to use your
     * own dialog you must redefine method "postInputConfigure()" to display your dialog, "savePostSettings(...)" and "restorePostSettings(...)" to save/restore post settings of your step.
     */
    virtual void fillPostInputConfigurationDialog(CT_StepConfigurableDialog* postInputConfigDialog) { Q_UNUSED(postInputConfigDialog)}

    /**
     * @brief Show the post configuration dialog.
     *
     * If you want to show your own configuration dialog your must overload this method and show your dialog when this method is called. Don't forget
     * to call the method "setSettingsModified(true)" if your settings is modified (if user accept your dialog).
     *
     * @return Returns false if the user cancel the configuration.
     */
    virtual bool postInputConfigure();

    /**
     * @brief Called after the post input dialog has been displayed and user accept the change OR after post settings has
     *        been restored
     */
    virtual void finalizePostSettings() {}

    /************ OUTPUTS DECLARATION ************/

    /**
     * @brief Called when the step must declare all output models. The manager must be used in
     *        conjunction with CT_HandleOutXXX to add output that the step will produce.
     */
    virtual void declareOutputModels(CT_StepOutModelStructureManager& manager) = 0;

    /************ PROCESSING (ALGORITHME, PROGRESSION, ERRORS, ETC...) ************/

    /**
     * @brief Redefine this method to make your process
     *
     * @warning Don't forget to call the method "isStopped()" in all loops to allow the user to stop the process.
     *          Don't forget to call the method "setProgress(...)" to inform the user the progression of your process.
     *          Don't forget to call the method "setErrorMessage(...)" if you have an error in your step. Or use the Log "STEP_LOG->add....".
     */
    virtual void compute() = 0;

    /**
     * @brief Redefine this method if your step allow modification of elements after the process is completed. The user can make a right click
     *        to your step and click "modify" so this method is called (not in GUI thread but in another thread). It is useful if you want to allow
     *        a user to modify elements manually (show "setManual" method for more information)
     *
     * @warning Use the method "setModifiable(true)" in your constructor to inform that this method can be called.
     *          Use the method "setManual(true)" in your constructor to make your step manual if you want that user can modify manually elements.
     */
    virtual void modify() {}

    /**
     * @brief Called this method in your "compute" method to inform the progress
     */
    void setProgress(float progress);

    /**
     * @brief Define an error message for an error code (map). Call this method in your constructor if you want.
     * @warning Don't use 0 because it will signify no error.
     */
    void setErrorMessage(int errorCode, const QString &errorMessage);

    /**
     * @brief Set the error code. 0 if it was no error, other if in your "compute" method there was and error.
     */
    void setErrorCode(int errorCode);

    /**
     * @brief Clear all error message
     */
    void clearErrorMessage();

    /************ DEBUG MODE ************/

    /**
     * @brief Define if your step is debuggable. Call this method in your constructor.
     * @param debuggable : true if you want to make your step debuggable
     */
    void setDebuggable(bool debuggable);

    /**
     * @brief If you set that this step is debuggable (method "setDebuggable(...)") you can call this method in your process loop to break it at a specified
     *        point. It will break only if user in the gui has activated the debug mode of this step with the help of the method "setDebugModeOn(true)".
     *
     *        The debug mode works like this (If user in the gui has activated the debug mode of this step) :
     *
     *        - When the processing is launch the break point number is set to 0 (you can acces to the current number with the method "currentDebugBreakPointNumber()").
     *        - When this method is called the current break point number is compared to the break point number to go :
     *
     *              * If they doesn't match => the thread is not stopped and your processing can continue as normal.
     *
     *              * If they match =>
     *                      --> The signal "waitForAckInDebugMode()" is emitted
     *                      --> The thread is stopped until one of this method is called : "ackDebugMode(...)", "stop()" or "setDebugModeOn(false)".
     *                      --> In same time the thread is stopped the gui capture the signal "waitForAckInDebugMode()" and call the method
     *                          "preWaitForAckIfInDebugMode()" in gui thread. (So you can add something in documents if you want per example).
     *                      --> When the user make an action to go to the next break point the method "postWaitForAckIfInDebugMode()" is called
     *                          first in gui thread.
     *                      --> Then the gui call the method "ackDebugMode(...)" and specify the number of call of the method "waitForAckIfInDebugMode()"
     *                          that it must be made before the next stop.
     *
     * @warning You must, in the constructor of your step, set that you use the debug mode with the method "setDebugModeOn(true)" otherwise
     *          this method does nothing !
     */
    void waitForAckIfInDebugMode();

public:
    /**
     * @brief Called by the gui in same time the thread of the step is stopped when a call to the method "waitForAckInDebugMode()" is made. By default do nothing.
     * @warning This method is called in the GUI thread ! So you can play with documents, etc...
     */
    virtual void preWaitForAckIfInDebugMode() {}

    /**
     * @brief Called by the gui when the user want to go to the next break point. By default do nothing.
     * @warning This method is called in the GUI thread ! So you can play with documents, etc...
     */
    virtual void postWaitForAckIfInDebugMode() {}

    /**
     * @brief Generate help file (html) for this step in specified directory
     */
    virtual QString generateHTMLDocumentation(QString directory, QString cssRelativeDirectory);

    /**
     * @brief Restore first step ID to 1. Must only be used if not step left in steptree
     */
    static void resetCurrentID();


protected:
    /**
     * @brief Returns the current number of the break point. In other words the number of turn that the debug mode has made.
     */
    int currentDebugBreakPointNumber() const;

    /**
     * @brief Returns the current number of break point to jump (set by the gui when it call "ackDebugMode(...)" method.
     */
    int currentNumberOfBreakPointToJump() const;

    /************ MANUAL MODE ************/

    /**
     * @brief Set if we can call the method "modify" of the step (call this method in your constructor with true if you plan to use the
     *        method "modify()")
     */
    void setModifiable(bool modifiableOn);

    /**
     * @brief Set if that step use the manual mode (call this method in your constructor with true if you plan to use the
     *        method "requestManualMode()"). The manual mode provides the ability to edit elements manually in documents
     *        via actions (see ActionInterface).
     */
    void setManual(bool manualModeOn);

    /**
     * @brief Call this method in your processing loop when you want to go in the manual mode.
     *
     *        The manual mode works like this :
     *
     *        - When this method is called :
     *
     *              * All input results that you use at this moment will be set un-busy
     *              * The signal "manualModeRequired()" is emitted
     *              * The thread is stopped until one of this method is called : "quitManualMode()" or "stop()".
     *              * In same time the thread is stopped the gui capture the signal "manualModeRequired()", set a gui context and
     *                call the method "ackManualMode()".
     *
     *                      --- > If it was the first time that the method "ackManualMode()" is called the method "initManualMode()" is called, otherwise
     *                            the method "useManualMode(false)" is called.
     *
     *              * When the user click the "hand" button in the gui the method "quitManualMode()" is called that call the method "useManualMode(true)".
     *
     * @warning You must, in the constructor of your step, set that you use the manual mode with the method "setManualMode(true)" otherwise
     *          this method does nothing !
     */
    void requestManualMode();

    /**
     * @brief Called, in the gui thread, when manual mode is activated for the first time (see method "requestManualMode()"
     *        for more details). Do nothing by default.
     *
     *        Redefine this method if you want to create documents and add actions to it.
     *
     * @example This is an example of a code that you can write in your step to create a new
     *          3D document and add it an action.
     *
     *          PMO_MyAction* action = new PMO_MyAction();
     *
     *          GuiContextInterface* context = this->getGuiContext();
     *          context->actionsManager()->addAction(action);
     *
     *          DocumentInterface* doc = context->documentManager()->new3DDocument();
     *          doc->setCurrentAction(action);
     */
    virtual void initManualMode();

    /**
     * @brief Called, in the gui thread, every time the manual mode is activated or when
     *        it must quit. Do nothing by default.
     *
     * @param quit : false if the method "ackManualMode()" is called after the first time, true when
     *               the method "quitManualMode()" is called.
     */
    virtual void useManualMode(bool quit = false);

private:
    using ChildrenCollection = QList<CT_VirtualAbstractStep*>;

    friend class CT_OutManager;
    friend class CT_InManager;

    class StepToolForModel : public IStepToolForModel {
    public:
        CT_VirtualAbstractStep* m_pointer;

        QString displayableCustomName() const override { return QString("(%2) %1").arg(m_pointer->description(), QString().setNum(m_pointer->uniqueID()));}
        bool isStepAPrototype() const override { return m_pointer->isAPrototype(); }
        bool visitStepHisOutResults(const OutResultsVisitor& visitor) const override {
            return m_pointer->visitOutResults([&visitor](const CT_AbstractResult* result) -> bool {
                return visitor(result);
            });
        }
    };

    static int                      CURRENT_ID;

    /**
     * @brief Unique ID
     */
    int                             m_uniqueId;

    /**
     * @brief The tool for model
     */
    StepToolForModel                m_stepTool;

    /**
     * @brief Parent step (must be nullptr if it was the root step or a prototype)
     */
    CT_VirtualAbstractStep*         m_parentStep;

    /**
     * @brief Plugin of the step
     */
    IPluginForStep*                 m_plugin;

    /**
     * @brief Displayable custom name
     */
    QString                         m_displayableCustomName;

    /**
     * @brief The unique index generator is only created by the root step. So it was shared
     *        with each child step.
     */
    UniqueIndexGeneratorPtr         m_uniqueIndexGenerator;

    /**
     * @brief Dialog that let the user configure settings before the configuration dialog of input result was displayed. It can
     *        be nullptr if there is no dialog to display.
     */
    CT_StepConfigurableDialog*      m_preInputConfigDialog;

    /**
     * @brief Dialog that let the user configure settings after the configuration dialog of input result was displayed and before
     *        output result model will be declared. It can be nullptr if there is no dialog to display.
     */
    CT_StepConfigurableDialog*      m_postInputConfigDialog;

    /**
     * @brief Collection of childrens
     */
    ChildrenCollection              m_childrens;

    /**
     * @brief Input manager
     */
    CT_InManager                    m_inputManager;

    /**
     * @brief Output manager
     */
    CT_OutManager                   m_outputManager;

    bool                            m_isRunning;               /*!< A true lorque l'étape est en cours de traitement */
    bool                            m_mustStop;                  /*!< Stop le traitement */
    int                             m_currentErrorCode;            /*!< code d'erreur (0 si pas d'erreur) */
    QMap<int, QString>              m_errorMessagesByErrorCode;       /*!< les messages d'erreurs en fonction du code d'erreur */

    QDateTime                       m_launchDateTime;           /*!< Date et heure de lancement du traitement */
    QElapsedTimer                   m_executeTime;          /*!< Calcul de la durée du traitement */
    int                             m_elapsedTimeInMs;               /*!< Durée effective du traitement */

    int                             m_currentProgressValue;              /*!< La progression */
    int                             m_currentClearMemoryProgressValue;      /*!< La progression lors de la suppression des résultats en mémoire */

    bool                            m_isSettingsModified;    /*!< les paramètres ont-ils été modifiées */

    bool                            m_isDebuggable;            /*!< si l'étape est débogable (false par défaut) */
    bool                            m_isDebugModeOn;             /*!< debug mode on/off */
    bool                            m_hasReceivedAckForDebugMode;        /*!< acquitement en mode debug */

    qint32                          m_currentBreakPointNumber;                 /*!< nombre de pas a pas (utile si on veut se reperer lorsqu'il y a un probleme dans l'algo) */
    int                             m_nBreakPointToJumpSetInAckDebugMode;                       /*!< store the value given by gui, for it to be accessible in the step (see CT_StepEndLoop for use exemple) */
    qint32                          m_nextBreakPointNumberToStop;             /*!< on va ignorer l'attente d'un ack du mode pas a pas tant que _step_by_step_size est inferieur a _index_step_step_to_go */

    bool                            m_isModifiable;                       /*!< is that step modifiable ? */
    bool                            m_isManual;                           /*!< is that step use the manual mode ? */
    bool                            m_isTheFirstCallToAckManualMode;            /*!< is the first call to the method ackManualMode() ? */
    bool                            m_mustQuitManualMode;               /*!< true if we must qui the manual mode */
    GuiContextInterface*            m_currentGuiContext;                      /*!< gui context in manual or debug mode */

    bool                            m_isAPrototype;

    /**
     * @brief Create a new standard pre configuration dialog. Call this method only once ! If you call this method multiple times the last
     *        standard pre configuration dialog will be deleted from memory.
     * @return The standard pre configuration dialog.
     */
    CT_StepConfigurableDialog* createOrGetStandardPreInputConfigurationDialog();

    /**
     * @brief Call to create and fill the pre input configuration dialog
     */
    void createPreInputConfigurationDialog();

    /**
     * @brief Call to create and fill the post input configuration dialog
     */
    void createPostInputConfigurationDialog();

    /**
     * @brief Recursively remove from collection and from memory out results of this step and them of childrens
     */
    void recursiveClearOutResult();

    /**
     * @brief Recursively remove from collection and from memory only out results of childrens
     */
    void recursiveClearChildOutResult();

    /**
     * @brief Recursively clear what is inside out results of this step and them of childrens
     */
    void recursiveClearOutResultFromMemory();

    /**
     * @brief Returns true if no input result models has been defined yet
     */
    bool mustDeclareInputModels() const;

    /**
     * @brief Returns true if no input result models has been defined yet and this step has no childrens
     */
    bool canDeclareInputModels() const;

    /**
     * @brief Called in "showInputResultConfigurationDialog()" or in "restoreInputSettings()" to initialize inputs models
     */
    bool internalDeclareInputModels();

    /**
     * @brief Set when the process has begun
     * @param launchTime : the current QDateTime
     */
    void setLaunchTime(const QDateTime& launchTime);

    /**
     * @brief Start or re-start the process loop.
     * @param modificationMode : if true clear only results of childrens recursively and call the method "modify()", otherwise
     *                           clear results of this step and of childrens recursively and call the method "compute()".
     */
    void runProcessing(bool modificationMode = false);

    /**
     * @brief Remove the specified result from the collection, clear it and delete it from the memory
     * @param n : index of the result to remove.
     */
    void removeOutResult(int n);

    /**
     * @brief Return the input manager
     */
    CT_InManager* inManager() const;

    /**
     * @brief return the output manager
     */
    CT_OutManager* outManager() const;

    /**
     * @brief Set the current progress value of the clear memory process.
     * @param progress : the current progress value.
     */
    void setClearMemoryProgress(int progress);

    /**
     * @brief Returns the unique index generator to use with output models
     */
    CT_UniqueIndexGenerator* uniqueIndexGenerator() const;


    /**
     * @brief Methods for creating inModels and outModels hierachy documentation
     */
    void recursiveCreateHelpStrForModel(QString &str, int nbTab, const CT_OutAbstractModel *rModel) const;
    void recursiveCreateHelpStrForModel(QString &str, int nbTab, const CT_InAbstractModel *rModel) const;

    void recursiveCreateHelpStrForResultModel(QString &str, int nbTab, const CT_OutAbstractResultModel *rModel) const;
    void recursiveCreateHelpStrForResultModel(QString &str, int nbTab, const CT_InAbstractResultModel *rModel) const;

    void recursiveCreateHelpStrForItemModel(QString &str, int nbTab, const CT_OutAbstractItemModel *iModel) const;
    void recursiveCreateHelpStrForItemModel(QString &str, int nbTab, const CT_InAbstractItemModel *iModel) const;

    void recursiveCreateHelpStrForItemAttributesModel(QString &str, int nbTab, const CT_OutAbstractItemAttributeModel *iaModel) const;
    void recursiveCreateHelpStrForItemAttributesModel(QString &str, int nbTab, const CT_InAbstractItemAttributeModel *iaModel) const;

    void createHelpStrForChildrens(QString &str, int nbTab, const CT_OutAbstractModel *model) const;
    void createHelpStrForChildrens(QString &str, int nbTab, const CT_InAbstractModel *model) const;


public slots:
    void openHelpPageForStep() const;

public:
    QUrl getHelpPageForStepPath() const;

    static void setCurrentLangage(QString lang) {CURRENT_LANGAGE = lang;}

    /**
     * @brief Export screen capture of IN model configuration dialog
     */
    bool exportViewForINModelConfig(QString exportPath, QStringList &exportedFiles);

private:
    static QString CURRENT_LANGAGE;

signals:

    /**
     * @brief Emitted when at least one setting has been modified
     */
    void settingsModified();

    /**
     * @brief Emitted when the displayable custom name of the step has been modified
     */
    void displayableCustomNameChanged();

    /**
     * @brief Emitted whenever the step enter in debug mode. The step wait that the gui call the method "ackDebugMode(...)" to continue the process.
     */
    void waitForAckInDebugMode();

    /**
     * @brief Emmited whenever the step enter in manual mode and required a manual mode context from GUI.
     */
    void manualModeRequired();

    /**
     * @brief Emitted when the step quit the manual mode or was stopped in manual mode.
     */
    void manualModeCompleted();

    /**
     * @brief Emitted when the step was in pause mode
     */
    void paused();

    /**
     * @brief Emitted when the step was stopped
     */
    void stopped();

    /**
     * @brief Emitted when the process is complete
     */
    void isCompleted();

    /**
     * @brief Emitted when the step want to show a message in gui
     */
    void showMessage(QString mess);

    /**
     * @brief Emitted when a output result is added to the collection
     * @param res : the output result
     */
    void resultAdded(const CT_AbstractResult* res);

    /**
     * @brief Emitted when the result will cleared its elements from memory
     * @param res : the resut
     * @warning if you want to use method of the result you must connect this signal in Qt::DirectConnection because
     *          the result can be removed from memory when this signal is catched in Qt::QueuedConnection
     */
    void resultToBeClearedFromMemory(const CT_AbstractResult* res);

    /**
     * @brief Emitted when the result will be removed from memory
     * @param res : the resut
     * @warning if you want to use method of the result you must connect this signal in Qt::DirectConnection because
     *          the result will be removed from memory if this signal is catched in Qt::QueuedConnection
     */
    void resultToBeRemoved(const CT_AbstractResult* res);
};

#endif // CT_VIRTUALABSTRACTSTEP_H

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

#ifndef CT_ABSTRACTSTEPPLUGIN_H
#define CT_ABSTRACTSTEPPLUGIN_H

#include "ct_step/tools/menu/ct_stepsmenu.h"
#include "ct_step/interfacesforstep.h"
#include "interfacesforplugin.h"

class CT_ActionsSeparator;
class CT_StandardExporterSeparator;
class CT_StandardReaderSeparator;

class CT_VirtualAbstractStep;
class CT_AbstractStepLoadFile;

class CT_AbstractFilter;
class CT_AbstractMetric;

class CT_AbstractItemDrawable;

class QSettings;

class CTLIBPLUGIN_EXPORT CT_AbstractStepPlugin : public IPluginForStep
{
public:
    CT_AbstractStepPlugin();
    virtual ~CT_AbstractStepPlugin();

    /**
     * @brief Must be called by the core or the gui to define the interface that can be used to dialog with the core. For all plugins
     *        loaded you must call this method.
     * @param core : the core of the application (can not be null ! and must be obligatory set)
     */
    void setCoreInterface(const CoreInterface* core);

    /**
     * @brief Must be called by the gui (if a gui exist) to define the interface that can be used to dialog with the gui. For all plugins
     *        loaded you must call this method.
     * @param gManager : the gui manager (can be null if it does not exist a gui)
     */
    void setGuiManager(const GuiManagerInterface* gManager);

    /**
     * @brief Initialize the plugin (load step / readers / exporters / actions / settings /etc....). Must be called from the core
     *        when the plugin is loaded.
     *
     *        If you want to save settings for your plugins show method "initQSettings()". This method will be called in the "init" method.
     *
     * @warning If you overload this method remember to call the method of the superclass !
     * @return true if the initialization is a success.
     */
    virtual bool init();

    /**
     * @brief Add other step in function of other plugins.
     * @return true if the initialization is a success.
     */
    bool initAfterAllPluginsLoaded();

    /**
     * @brief Initialize step / readers / exporters / actions / etc.... Must be called from the core for each plugins AFTER ALL plugins was loaded
     */
    void finishInitialization();

    /**
     * @brief Called from plugin manager when this plugin will be unloaded
     */
    virtual void unload();

    /**
     * @brief Returns a list of action separator. A action separator group multiple action by a title. This
     *        separator contain standard action that can be installed on GraphicsViewInterface / TreeViewInterface / etc...
     */
    QList<CT_ActionsSeparator*> getActionsAvailable() const;

    /**
     * @brief Returns a list of exporter separator. A exporter separator group multiple exporter by a title. This
     *        separator contain standard exporters that can export items / points / faces /edges / etc...
     */
    QList<CT_StandardExporterSeparator*> getExportersAvailable() const;

    /**
     * @brief Returns a list of reader separator. A reader separator group multiple reader by a title. This
     *        separator contain standard readers that can read file / stream / etc... to generate items
     */
    QList<CT_StandardReaderSeparator *> getReadersAvailable() const;

    /**
     * @brief Returns a list of filters available in this plugin.
     */
    QList<CT_AbstractFilter *> getFiltersAvailable() const;

    /**
     * @brief Returns a list of metrics available in this plugin.
     */
    QList<CT_AbstractMetric *> getMetricsAvailable() const;

    /**
     * @brief Returns a list of itemdrawable available in this plugin.
     */
    QList<CT_AbstractItemDrawable *> getItemDrawablesAvailable() const;

    using ItemDrawableVisitor = std::function<bool(const CT_AbstractItemDrawable*)>;

    /**
     * @brief Call it to visit items registered in this plugin.
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_AbstractItemDrawable and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool visitRegisteredItemDrawable(const ItemDrawableVisitor& visitor) const;

    /**
     * @brief Returns a list of step that can read the file define by the filepath passed in parameter
     * @warning If you overload this method remember to call the method of the superclass !
     */
    virtual QList<CT_AbstractStepLoadFile*> getOpenFileStep(const QString &filePath) const;

    /**
     * @brief Retuns a key to retrieve the step passed in parameter later (if you load a script per example). Used
     *        the method "getStepFromKey" to get the step from a key.
     * @warning If you overload this method remember to call the method of the superclass !
     */
    virtual QString getKeyForStep(const CT_VirtualAbstractStep &step) const;

    /**
     * @brief Returns the step that correspond to the key passed in parameter. Used the method "getKeyForStep" if you want
     *        to get a key from a step.
     * @warning If you overload this method remember to call the method of the superclass !
     */
    virtual CT_VirtualAbstractStep* getStepFromKey(QString key) const;

    /**
     * @brief Create a copy of the step (model) passed in parameter
     * @param step : the step to copy
     * @param parent : the future parent of the step (can be nullptr if the step must be root)
     * @return nullptr if the copy is impossible
     * @warning If you overload this method remember to call the method of the superclass !
     */
    virtual CT_VirtualAbstractStep* createNewInstanceOfStep(const CT_VirtualAbstractStep &step, CT_VirtualAbstractStep *parent, bool isAPrototype = false) const;

    /**
     * @brief Returns true if this plugin can be configured. By default returns false.
     * @warning If you overload this method remember to call the method of the superclass !
     */
    virtual bool canBeConfigured() const { return false; }

    /**
     * @brief Configure this plugin and returns true if the configuration is accepted. By default returns false.
     * @warning If you overload this method remember to call the method of the superclass !
     */
    virtual bool configure() { return false; }

    /**
     * @brief Get the official plugin url (= project ex.: https://computree.onf.fr/?page_id=1344 for Plugin Base)
     * @return Project URL
     */
    virtual QString getPluginURL() const = 0;

    /**
     * @brief Get the official plugin name
     * @return Plugin official name
     */
    virtual QString getPluginOfficialName() const {return "";}

    /**
     * @brief Get the plugin official citation in RIS format
     * @return Plugin official citation in RIS format
     */
    virtual QString getPluginRISCitation() const {return "";}
    virtual QStringList getPluginRISCitationList() const {return QStringList() << getPluginRISCitation();}

    IPluginToolForStep* pluginToolForStep() const final;

protected:

    /**
     * @brief Overload this method to load your standard step. Use "addNewSeparator" method to create a new separator and a step to it.
     * @return true if load is a success.
     * @warning Method called in the "init" method. If you must wait that all plugins is loaded to create
     *          your elements prefer overload the method "loadAfterAllPluginsLoaded"
     */
    virtual bool loadGenericsStep() = 0;

    /**
     * @brief Overload this method to load your step that can load file. Use "addNewSeparator" method to create a new separator and a step to it.
     * @return true if load is a success.
     * @warning Method called in the "init" method. If you must wait that all plugins is loaded to create
     *          your elements prefer overload the method "loadAfterAllPluginsLoaded"
     */
    virtual bool loadOpenFileStep() = 0;

    /**
     * @brief Overload this method to load your step that can be added first. Use "addNewSeparator" method to create a new separator and a step to it.
     * @return true if load is a success.
     * @warning Method called in the "init" method. If you must wait that all plugins is loaded to create
     *          your elements prefer overload the method "loadAfterAllPluginsLoaded"
     */
    virtual bool loadCanBeAddedFirstStep() = 0;

    /**
     * @brief Overload this method to load your actions. Use "addNewSeparator" method to create a new separator and a actions to it.
     * @return true if load is a success.
     * @warning Method called in the "init" method. If you must wait that all plugins is loaded to create
     *          your elements prefer overload the method "loadAfterAllPluginsLoaded"
     */
    virtual bool loadActions() = 0;

    /**
     * @brief Overload this method to load your exporters. Use "addNewSeparator" method to create a new separator and a exporters to it.
     * @return true if load is a success.
     * @warning Method called in the "init" method. If you must wait that all plugins is loaded to create
     *          your elements prefer overload the method "loadAfterAllPluginsLoaded"
     */
    virtual bool loadExporters() = 0;

    /**
     * @brief Overload this method to load your readers. Use "addNewSeparator" method to create a new separator and a readers to it.
     * @return true if load is a success.
     * @warning Method called in the "init" method. If you must wait that all plugins is loaded to create
     *          your elements prefer overload the method "loadAfterAllPluginsLoaded"
     */
    virtual bool loadReaders() = 0;

    /**
     * @brief Overload this method to load your filters. Use "addNewSeparator" method to create a new separator and a filter to it.
     * @return true if load is a success.
     * @warning Method called in the "init" method. If you must wait that all plugins is loaded to create
     *          your elements prefer overload the method "loadAfterAllPluginsLoaded"
     */
    virtual bool loadFilters();

    /**
     * @brief Overload this method to load your metrics. Use "addNewSeparator" method to create a new separator and a metrics to it.
     * @return true if load is a success.
     * @warning Method called in the "init" method. If you must wait that all plugins is loaded to create
     *          your elements prefer overload the method "loadAfterAllPluginsLoaded"
     */
    virtual bool loadMetrics();

    /**
     * @brief Overload this method to register your itemdrawables. Use "addNewItemDrawable" method to register it.
     * @return true if load is a success.
     * @warning Method called in the "init" method. If you must wait that all plugins is loaded to create
     *          your elements prefer overload the method "loadAfterAllPluginsLoaded"
     */
    virtual bool loadItemDrawables();

    /**
     * @brief Inherit this method to add your step, actions, exporters, etc... after all plugins was loaded (if you must use elements from
     *        another plugin). Use "addNewSeparator" method to create a new separator and a elements to it.
     */
    virtual bool loadAfterAllPluginsLoaded() { return true; }

    /**
     * @brief Inherit this method to delete step that have been created in method loadAfterAllPluginsLoaded()
     */
    virtual void aboutToBeUnloaded() { }

    /**
     * @brief Remove from memory all actions
     */
    void clearActions();

    /**
     * @brief Remove from memory all exporters
     */
    void clearExporters();

    /**
     * @brief Remove from memory all readers
     */
    void clearReaders();

    /**
     * @brief Remove from memory all readers
     */
    void clearFilters();

    /**
     * @brief Remove from memory all readers
     */
    void clearMetrics();

    /**
     * @brief Remove from memory all itemdrawables
     */
    void clearItemDrawables();

    /**
     * @brief Overload this method if you want to return a new QSettings that will be added to the CT_StepInitializeData. So you can
     *        save some settings of your steps (like the last filepath opened, etc...). Settings will be automatically saved when the
     *        QSettings object is destroyed. This class manage the life in memory of the QSettings object.
     *
     * @return nullptr by default. A new QSettings if you want to save settings.
     */
    virtual QSettings* initQSettings() { return nullptr; }

    /**
     * @brief Search the steps (of this plugin) with key passed in parameter in level and sub level of the menu recursively and return it
     *        if he found it otherwise return nullptr
     */
    CT_VirtualAbstractStep* searchStepFromKeyOfThisPluginInMenuRecursively(CT_StepsMenu *menu, const QString &key) const;

    /**
     * @brief Search the steps (of this plugin) with key passed in parameter in level and sub level recursively and return it
     *        if he found it otherwise return nullptr
     */
    CT_VirtualAbstractStep* searchStepFromKeyOfThisPluginInLevelsRecursively(CT_MenuLevel *level, const QString &key) const;

    /**
     * @brief Create a new separator that can contains action. You must pass a new object to this method, the object
     *        is added to the list of separator and returned.
     */
    CT_ActionsSeparator* addNewSeparator(CT_ActionsSeparator *sep);

    /**
     * @brief Create a new separator that can contains exporter. You must pass a new object to this method, the object
     *        is added to the list of separator and returned.
     */
    CT_StandardExporterSeparator* addNewSeparator(CT_StandardExporterSeparator *sep);

    /**
     * @brief Create a new separator that can contains reader. You must pass a new object to this method, the object
     *        is added to the list of separator and returned.
     */
    CT_StandardReaderSeparator* addNewSeparator(CT_StandardReaderSeparator *sep);

    /**
     * @brief Add a new Filter to filters list.
     */
    void addNewFilter(CT_AbstractFilter *filter);

    /**
     * @brief Add new Metric to metrics list.
     */
    void addNewMetric(CT_AbstractMetric *metric);

    /**
     * @brief Add a new step that can be classified with a type of operation.
     * @info Prefer use the method "menuOfSteps" that returns the menu where you can customize level it's name
     *       level.
     */
    void addNewStep(CT_VirtualAbstractStep *step, CT_StepsMenu::LevelOperationType levelOperation, const QString &subLevelDisplayableName);
    void addNewStep(CT_VirtualAbstractStep *step, CT_StepsMenu::LevelOperationType levelOperation, CT_StepsMenu::LevelPredefined subLevelPredefined);

    /**
     * @brief Add a new step that can be classified with a type of operation.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewStep(CT_StepsMenu::LevelOperationType levelOperation, const QString &subLevelDisplayableName = "") {
        addNewStep(new STEP(), levelOperation, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewStep(CT_StepsMenu::LevelOperationType levelOperation, CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep(new STEP(), levelOperation, subLevelPredefined);
    }

    /**
     * @brief Add a new "load" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewLoadStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_Load, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewLoadStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_Load, subLevelPredefined);
    }

    /**
     * @brief Add a new "export" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewExportStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_Export, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewExportStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_Export, subLevelPredefined);
    }

    /**
     * @brief Add a new "points" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewPointsStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_Points, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewPointsStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_Points, subLevelPredefined);
    }

    /**
     * @brief Add a new "geometrical shapes" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewGeometricalShapesStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_GeometricalShapes, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewGeometricalShapesStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_GeometricalShapes, subLevelPredefined);
    }

    /**
     * @brief Add a new "shapes 2D" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewShapes2DStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_Shapes2D, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewShapes2DStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_Shapes2D, subLevelPredefined);
    }

    /**
     * @brief Add a new "voxels" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewVoxelsStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_Voxels, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewVoxelsStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_Voxels, subLevelPredefined);
    }

    /**
     * @brief Add a new "rasters" step.
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewRastersStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_Rasters, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewRastersStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_Rasters, subLevelPredefined);
    }

    /**
     * @brief Add a new "meshes" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewMeshesStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_Meshes, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewMeshesStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_Meshes, subLevelPredefined);
    }

    /**
     * @brief Add a new "metrics" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewMetricStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_Metrics, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewMetricStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_Metrics, subLevelPredefined);
    }

    /**
     * @brief Add a new "workflow" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewWorkflowStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_WorkFlow, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewWorkflowStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_WorkFlow, subLevelPredefined);
    }

    /**
     * @brief Add a new "other" step.
     * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
     * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
     *       level it's name.
     */
    template<class STEP>
    void addNewOtherStep(const QString &subLevelDisplayableName = "") {
        addNewStep<STEP>(CT_StepsMenu::LO_Other, subLevelDisplayableName);
    }

    template<class STEP>
    void addNewOtherStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
        addNewStep<STEP>(CT_StepsMenu::LO_Other, subLevelPredefined);
    }

    /**
    * @brief Add a new "beta" step.
    * @param subLevelDisplayableName : Can be empty if you want to add your step in the root level or can be customized to add it to a sub level with this name
    * @info If you want to add a custom level under the operation prefer use the method "menuOfSteps" that returns the menu where you can customize
    *       level it's name.
    */
   template<class STEP>
   void addNewBetaStep(const QString &subLevelDisplayableName = "") {
       addNewStep<STEP>(CT_StepsMenu::LO_Beta, subLevelDisplayableName);
   }

   template<class STEP>
   void addNewBetaStep(CT_StepsMenu::LevelPredefined subLevelPredefined) {
       addNewStep<STEP>(CT_StepsMenu::LO_Beta, subLevelPredefined);
   }

    /**
     * @brief Returns the menu for add level and steps
     */
    CT_StepsMenu* menuOfSteps() const;

    /**
     * @brief Register a itemdrawable that was created by your plugin
     */
    template<class ITEM>
    void addNewItemDrawable() {
        m_items.append(new ITEM());
    }

    /**
     * @brief Clear all steps / actions / readers / exporters / etc... from this plugins (call methods "clearXXX")
     */
    virtual void clearMemory();

private:
    class PluginToolForStep : public IPluginToolForStep {
    public:
        virtual QString url() const { return m_pointer->getPluginURL(); }
        virtual QString officialName() const { return m_pointer->getPluginOfficialName(); }

        CT_AbstractStepPlugin* m_pointer;
    };

    PluginToolForStep                           m_tool;
    QList<CT_ActionsSeparator*>                 m_actions;
    QList<CT_StandardExporterSeparator*>        m_exporters;
    QList<CT_StandardReaderSeparator*>          m_readers;
    QList<CT_AbstractFilter*>                   m_filters;
    QList<CT_AbstractMetric*>                   m_metrics;
    QList<CT_AbstractItemDrawable*>             m_items;

    QSettings                                   *_pluginSettings;

    friend class CT_MenuLevel;

    /**
     * @brief Initialize the step passed in parameter. Call this method after all plugins is loaded !
     * @param step : the step to init
     */
    void initStep(CT_VirtualAbstractStep *step) const;

    /**
     * @brief Init all steps of this plugin in levels of the general menu
     */
    void initAllStepOfThisPluginInLevelsRecursively(CT_MenuLevel *level);

    /**
     * @brief Search all steps of this plugin in levels of the general menu that accept to open the file passed in parameter
     */
    void searchOpenFileStepInLevelRecursively(CT_MenuLevel *level,
                                              const QString &filepath,
                                              QList<CT_AbstractStepLoadFile*> &listOfStepThatAcceptThisFile,
                                              QList<CT_AbstractStepLoadFile*> &listOfStepThatAcceptAllFiles) const;
};

#endif // CT_ABSTRACTSTEPPLUGIN_H

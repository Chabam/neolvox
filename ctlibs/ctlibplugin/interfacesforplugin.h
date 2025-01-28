#ifndef INTERFACESFORPLUGIN_H
#define INTERFACESFORPLUGIN_H

#include <QString>

class CT_AbstractStepPlugin;
class CT_StepsMenu;
class CT_AbstractAction;
class LogInterface;
class DocumentManagerInterface;

class ActionsManagerInterface
{
public:
    virtual ~ActionsManagerInterface() {}

    /**
     * @brief Add a new action. The manager is responsible of the life of the action. You must not delete
     *        your action ! The action is deleted if you remove it or if the manager is destroyed or if the
     *        manual or debug mode is finished.
     *
     *        Before set the current action to a document you must add it in the actions manager by this method.
     *
     * @return return false if the action can't be added because already exist in a plugins.
     */
    virtual bool addAction(CT_AbstractAction *action) = 0;

    /**
     * @brief Remove the action and the associated button from the view. Action is deleted after it has been removed.
     *
     * @warning if you remove a action that is activate in a document it will be finished and deleted.
     *
     * @return return false if it can't be removed because it's a action from a plugin or if she don't exist in the list.
     */
    virtual bool removeAction(CT_AbstractAction *action) = 0;
    virtual bool removeAction(const QString &uniqueName) = 0;

    /**
     * @brief Return true if the action exist in the actions added by a step or available in a plugin
     */
    virtual bool existAction(const CT_AbstractAction *action) const = 0;
    virtual bool existAction(const QString &uniqueName) const = 0;

    /**
     * @brief Return the action that have this unique name. NULL if not exist.
     */
    virtual CT_AbstractAction* action(const QString &uniqueName) const = 0;
};

/**
 * @brief Represent the manager of plugins
 */
class PluginManagerInterface
{
public:
    virtual ~PluginManagerInterface() {}

    /*!
     *  \brief return true if at least one plugin is loaded
     */
    virtual bool isAPluginLoaded() const = 0;

    /*!
     *  \brief return the number of plugins loaded
     */
    virtual int countPluginLoaded() const = 0;

    /*!
     *  \brief return the name of the plugin 'i'
     */
    virtual QString getPluginName(int i = 0) const = 0;

    /*!
     *  \brief Retourne le nom du plugin en fonction de son CT_AbstractStepPlugin.
     */
    virtual QString getPluginName(CT_AbstractStepPlugin *stepPluginManager) const = 0;

    /*!
     *  \brief return the search path for plugins
     */
    virtual QString getSearchPath() const = 0;

    /*!
     *  \brief return the plugin 'i'
     */
    virtual CT_AbstractStepPlugin* getPlugin(int i) const = 0;

    /*!
     *  \brief return the plugin with name 'pluginName'
     */
    virtual CT_AbstractStepPlugin* getPlugin(QString pluginName) const = 0;
};

/**
 * @brief Représente le gestionnaire de l'application (la vue)
 */
class GuiManagerInterface
{
public:
    virtual ~GuiManagerInterface() {}

    /**
     * @return le gestionnaire des documents. NULL si il n'y en a pas.
     */
    virtual DocumentManagerInterface* documentManager() const = 0;
};

/**
 * @brief Represent the core of the application
 */
class CoreInterface
{
public:
    virtual ~CoreInterface() {}

    /**
     * @brief get the log
     */
    virtual LogInterface* log() const = 0;

    /**
     * @brief get the plugin manager
     */
    virtual PluginManagerInterface* pluginManager() const = 0;

    /**
     * @brief get the menu to populate with steps
     */
    virtual CT_StepsMenu* stepsMenu() const = 0;
};

#endif // INTERFACESFORPLUGIN_H

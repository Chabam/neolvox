#ifndef CDM_PLUGINMANAGER_H
#define CDM_PLUGINMANAGER_H

#include "computreeCore_global.h"

#include "interfacesforplugin.h"

#include <QPluginLoader>

#include "cdm_log.h"
#include "cdm_stepsmenumanager.h"

#include "ct_step/tools/menu/ct_stepsmenu.h"

/**
  * \brief Classe permettant de charger/gerer les plugins disponibles
  */
class COMPUTREECORESHARED_EXPORT CDM_PluginManager : public QObject, public CoreInterface, public PluginManagerInterface
{
    Q_OBJECT

public:

    CDM_PluginManager();
    ~CDM_PluginManager();

    /**
     * @brief Définir le gestionnaire de l'application (vue) afin de le partager au plugins
     */
    void setGuiManager(const GuiManagerInterface *gManager);

    /**
     * @brief get the log
     */
    LogInterface* log() const;

    /**
     * @brief get the plugin manager
     */
    PluginManagerInterface* pluginManager() const;

    /**
     * @brief get the menu to fill with steps and level
     */
    CT_StepsMenu* stepsMenu() const;

    /**
     * @brief get the menu manager
     */
    CDM_StepsMenuManager* stepsMenuManager() const;

    /*!
     *  \brief Charge/Recharge les plugins.
     *
     *  \return false si le chargement a echoue. Voir getError() pour plus d'informations.
     */
    bool load();

    /*!
     *  \brief Retourne un message expliquant la derniere erreur de chargement connu.
     */
    QString getError() const;

    /*!
     *  \brief Retourne true si au moins un plugin a ete charge.
     */
    bool isAPluginLoaded() const;

    /*!
     *  \brief Retourne le nombre de plugin charge.
     */
    int countPluginLoaded() const;

    /*!
     *  \brief Retourne le nom du plugin "i"
     */
    QString getPluginName(int i = 0) const;

    /*!
     *  \brief Retourne le nom du plugin en fonction de son CT_AbstractStepPlugin.
     */
    QString getPluginName(CT_AbstractStepPlugin *stepPluginManager) const;

    /*!
     *  \brief Retourne le chemin de recherche des plugins.
     */
    QString getSearchPath() const;

    /*!
     *  \brief Defini le chemin de recherche des plugins.
     */
    void setSearchPath(QString path);

    /*!
     *  \brief Retourne l'entree du plugin "i".
     */
    CT_AbstractStepPlugin* getPlugin(int i) const;

    /*!
     *  \brief Retourne l'entree du plugin en fonction de son nom.
     */
    CT_AbstractStepPlugin* getPlugin(QString pluginName) const;

    /**
     * @brief Returns the index of the plugin in the list
     */
    int getPluginIndex(CT_AbstractStepPlugin *p) const;       

private:

    QMap<QString, QPluginLoader*>       _plugins;

    QString                             _defaultPluginDirPath;
    QString                             _errorWhenLoad;

    GuiManagerInterface                 *m_guiManager;
    CDM_Log                             m_log;

    CDM_StepsMenuManager                m_stepsMenuManager;

    void clearPlugins();

    void loadConfiguration();
    void writeConfiguration();

signals:

    /**
     * @brief Start to loading. You can change the value of the "cancel" variable to true
     *        if you want to cancel the loading (connect with Qt::DirectConnection)
     */
    void startLoading(bool &cancel);
    void beginLoading();
    void finishLoading();
};

#endif // CDM_PLUGINMANAGER_H

#ifndef PLUGINENTRYINTERFACE_H
#define PLUGINENTRYINTERFACE_H

#include <QObject>

class CT_AbstractStepPlugin;

/*!
 *  \brief Représente le point d'entrée du Plugin.
 */
class PluginEntryInterface : public QObject
{
    Q_OBJECT

public:

    virtual ~PluginEntryInterface() {}

    /*!
     *  \brief Retourne le numro de version du plugin.
     */
    virtual QString getVersion() const = 0;

    /*!
     *  \brief Retourne le gestionnaire de plugin d'étapes
     */
    virtual CT_AbstractStepPlugin* getPlugin() const = 0;
};

#define PluginEntryInterface_iid "com.krebs.michael.ONF.ctlibplugin.PluginEntryInterface"

Q_DECLARE_INTERFACE(PluginEntryInterface, PluginEntryInterface_iid)

#endif // PLUGINENTRYINTERFACE_H

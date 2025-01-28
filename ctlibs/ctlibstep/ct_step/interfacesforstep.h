#ifndef INTERFACESFORSTEP_H
#define INTERFACESFORSTEP_H

#include <QString>
#include <QObject>

class IPluginToolForStep {
public:
    virtual ~IPluginToolForStep() {}
    virtual QString url() const = 0;
    virtual QString officialName() const = 0;
};

class IPluginForStep {
public:
    virtual ~IPluginForStep() {}

    virtual IPluginToolForStep* pluginToolForStep() const = 0;
};

class DocumentManagerInterface;
class ActionsManagerInterface;
class CT_VirtualAbstractStep;

/**
 * @brief Represent a gui context
 */
class GuiContextInterface
{
public:
    virtual ~GuiContextInterface() {}

    /**
     * @brief Return the document manager
     */
    virtual DocumentManagerInterface* documentManager() const = 0;

    /**
     * @brief Return the action manager
     */
    virtual ActionsManagerInterface* actionsManager() const = 0;
};

#endif // INTERFACESFORSTEP_H

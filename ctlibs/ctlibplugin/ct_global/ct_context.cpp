#include "ct_context.h"

#include "ct_log/ct_logmanager.h"

CT_Context* CT_Context::UNIQUE_INSTANCE = nullptr;

CT_Context::CT_Context() :
    m_guiManager(nullptr),
    m_coreInterface(nullptr)
{
}

CoreInterface* CT_Context::core() const
{
    return m_coreInterface;
}

GuiManagerInterface* CT_Context::guiManager() const
{
    return m_guiManager;
}

LogInterface* CT_Context::log() const
{
    return CT_LogManager::staticInstance()->currentLogInterface();
}

PluginManagerInterface* CT_Context::pluginManager() const
{
    return (m_coreInterface == nullptr) ? nullptr : m_coreInterface->pluginManager();
}

DocumentManagerInterface* CT_Context::documentManager() const
{
    return (m_guiManager == nullptr) ? nullptr : m_guiManager->documentManager();
}

void CT_Context::setCore(const CoreInterface* core)
{
    m_coreInterface = const_cast<CoreInterface*>(core);

    CT_LogManager::staticInstance()->setLogInterfaceToUse(m_coreInterface->log());
}

void CT_Context::setGuiManager(const GuiManagerInterface* gManager)
{
    m_guiManager = const_cast<GuiManagerInterface*>(gManager);
}

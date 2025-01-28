#ifndef CT_CONTEXT_H
#define CT_CONTEXT_H

#include "ctlibplugin_global.h"

#include "ct_global/ct_structurecontext.h"
#include "ct_global/ct_cloudscontext.h"
#include "interfacesforplugin.h"

#ifndef PS_LOG
#define PS_LOG CT_Context::staticInstance()->log()
#endif

class CTLIBPLUGIN_EXPORT CT_Context
{
public:
    static CT_Context* staticInstance()
    {
        if(UNIQUE_INSTANCE == nullptr)
            UNIQUE_INSTANCE = new CT_Context();

        return UNIQUE_INSTANCE;
    }

    static void deleteStaticInstance()
    {
        delete UNIQUE_INSTANCE;
        UNIQUE_INSTANCE = nullptr;
    }

    /**
     * @brief Returns the core (CORE)
     */
    CoreInterface* core() const;

    /**
     * @brief Returns the gui manager (GUI)
     */
    GuiManagerInterface* guiManager() const;

    /**
     * @brief Returns the log (CORE)
     */
    LogInterface* log() const;

    /**
     * @brief Returns the plugin manager (CORE)
     */
    PluginManagerInterface* pluginManager() const;

    /**
     * @brief Returns the document manager (GUI)
     */
    DocumentManagerInterface* documentManager() const;

    /**
     * @brief Set the core interface to use
     * @warning Not intended to be used by developper
     */
    void setCore(const CoreInterface* core);

    /**
     * @brief Set the gui interface to use
     * @warning Not intended to be used by developper
     */
    void setGuiManager(const GuiManagerInterface* gManager);

private:
    static CT_Context*      UNIQUE_INSTANCE;
    GuiManagerInterface*    m_guiManager;
    CoreInterface*          m_coreInterface;

    CT_Context();
};

#endif // CT_CONTEXT_H

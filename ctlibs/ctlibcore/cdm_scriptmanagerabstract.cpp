#include "cdm_scriptmanagerabstract.h"

CDM_ScriptManagerAbstract::CDM_ScriptManagerAbstract(CDM_PluginManager &pluginManager)
{
    m_callBack = nullptr;
    setPluginManager(pluginManager);
}

CDM_ScriptManagerAbstract::IScriptLoadCallBack *CDM_ScriptManagerAbstract::scriptLoadCallBack() const
{
    return m_callBack;
}

void CDM_ScriptManagerAbstract::setScriptLoadCallBack(CDM_ScriptManagerAbstract::IScriptLoadCallBack *c)
{
    m_callBack = c;

    scriptLoadCallBackChanged();
}

void CDM_ScriptManagerAbstract::setPluginManager(CDM_PluginManager &pluginManager)
{
    _pluginManager = &pluginManager;
}

CDM_PluginManager* CDM_ScriptManagerAbstract::getPluginManager() const
{
    return _pluginManager;
}

bool CDM_ScriptManagerAbstract::acceptFile(const QString &filePath) const
{
    QList<QString> extList = getFileExtensionAccepted();
    QListIterator<QString> it(extList);

    while(it.hasNext())
    {
        QString ext = it.next();

        if(filePath.lastIndexOf(ext) == (filePath.size()-ext.size()))
            return true;
    }

    return false;
}

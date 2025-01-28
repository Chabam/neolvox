#ifndef COMPUTREECOREDEFAULTIOCMD_H
#define COMPUTREECOREDEFAULTIOCMD_H

#include <QString>

#define STR(x)                                  #x
#define XSTR(x)                                 STR(x)
#define XSTR_TO_QSTRING(x)                      QString(XSTR(x))

#define IOCmd_RESPONSE_OK_BEGIN                 <OK>
#define IOCmd_RESPONSE_OK_END                   </OK>

#define IOCmd_RESPONSE_ERROR_BEGIN              <ERROR>
#define IOCmd_RESPONSE_ERROR_END                </ERROR>

#define IOCmd_STEPMANAGER_COMPLETED_BEGIN       <STEPMANCOMPLETED>
#define IOCmd_STEPMANAGER_COMPLETED_END         </STEPMANCOMPLETED>

#define IOCmd_REQUEST_GET_PLUGIN_DIR_BEGIN      <PLUGINDIR?>
#define IOCmd_REQUEST_GET_PLUGIN_DIR_END

#define IOCmd_REQUEST_RELOAD_PLUGIN_BEGIN       <PLUGINDIR>
#define IOCmd_REQUEST_RELOAD_PLUGIN_END

#define IOCmd_REQUEST_START_STEP_MANAGER_BEGIN  <STARTSTEPMAN>
#define IOCmd_REQUEST_START_STEP_MANAGER_END

#endif // COMPUTREECOREDEFAULTIOCMD_H

QT += xml

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)

TARGET = ctlibcore

DEFINES += COMPUTREECORE_LIBRARY

HEADERS += cdm_configfile.h \
    cdm_steplistdocexporter.h \
    cdm_steplistinfomanager.h \
    computreeCore_global.h \
    cdm_stepmanageroptions.h \
    cdm_stepmanager.h \
    cdm_pluginmanager.h \
    cdm_scriptmanagerabstract.h \
    cdm_stdiolistener.h \
    cdm_stdioparser.h \
    parserAction/cdm_parseractionmanager.h \
    parserAction/cdm_iparseraction.h \
    parserAction/cdm_abstractparseraction.h \
    parserAction/cdm_parseractiongetplugindir.h \
    parserAction/cdm_parseractionsetplugindir.h \
    computreeCoreDefaultIOCmd.h \
    parserAction/cdm_parseractionlaunchstepmanager.h \
    cdm_actionsmanager.h \
    cdm_log.h \
    cdm_tools.h \
    cdm_internationalization.h \
    cdm_scriptproblem.h \
    cdm_stepsmenumanager.h \
    cdm_hrscriptmanagerxml.h \
    cdm_hrscriptxmlwriter.h \
    cdm_hrscriptxmlreader.h \
    cdm_xmltools.h \
    cdm_scriptmanagerxmlallversions.h

SOURCES += \
    cdm_configfile.cpp \
    cdm_steplistdocexporter.cpp \
    cdm_steplistinfomanager.cpp \
    cdm_stepmanageroptions.cpp \
    cdm_stepmanager.cpp \
    cdm_pluginmanager.cpp \
    cdm_scriptmanagerabstract.cpp \
    cdm_stdiolistener.cpp \
    cdm_stdioparser.cpp \
    parserAction/cdm_parseractionmanager.cpp \
    parserAction/cdm_abstractparseraction.cpp \
    parserAction/cdm_parseractiongetplugindir.cpp \
    parserAction/cdm_parseractionsetplugindir.cpp \
    parserAction/cdm_parseractionlaunchstepmanager.cpp \
    cdm_actionsmanager.cpp \
    cdm_log.cpp \
    cdm_tools.cpp \
    cdm_internationalization.cpp \
    cdm_scriptproblem.cpp \
    cdm_stepsmenumanager.cpp \
    cdm_hrscriptmanagerxml.cpp \
    cdm_hrscriptxmlwriter.cpp \
    cdm_hrscriptxmlreader.cpp \
    cdm_xmltools.cpp \
    cdm_scriptmanagerxmlallversions.cpp

TRANSLATIONS += languages/ctlibcore_fr.ts \
                languages/ctlibcore_en.ts

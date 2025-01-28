QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)

TARGET = ctlibplugin

DEFINES += CTLIBPLUGIN_LIBRARY

HEADERS += \
    ctlibplugin_global.h \
    ct_abstractstepplugin.h \
    ct_exporter/private/ct_standardexporterseparator_p.h \
    ct_reader/ct_standardreaderseparator.h \
    ct_exporter/ct_standardexporterseparator.h \
    ct_step/tools/menu/ct_menulevel.h \
    ct_step/tools/menu/ct_stepsmenu.h \
    ct_actions/ct_actionsseparator.h \
    ct_global/ct_context.h \
    pluginentryinterface.h

SOURCES += \ 
    ct_abstractstepplugin.cpp \
    ct_reader/ct_standardreaderseparator.cpp \
    ct_exporter/ct_standardexporterseparator.cpp \
    ct_step/tools/menu/ct_menulevel.cpp \
    ct_step/tools/menu/ct_stepsmenu.cpp \
    ct_actions/ct_actionsseparator.cpp \
    ct_global/ct_context.cpp

TRANSLATIONS += languages/ctlibplugin_fr.ts \
                languages/ctlibplugin_en.ts

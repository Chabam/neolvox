QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)

TARGET = ctlibaction

DEFINES += CTLIBACTION_LIBRARY

HEADERS += \
    ctlibaction_global.h \
    ct_actions/abstract/ct_abstractaction.h \
    ct_actions/actionoptionsinterface.h \
    ct_actions/view/abstract/ct_gabstractactionoptions.h \
    ct_actions/abstract/ct_abstractactionforgraphicsview.h \
    ct_actions/abstract/ct_abstractactionfortreeview.h \
    ct_actions/view/abstract/ct_gabstractactionforgraphicsviewoptions.h

SOURCES += \
    ct_actions/abstract/ct_abstractaction.cpp \
    ct_actions/view/abstract/ct_gabstractactionoptions.cpp \
    ct_actions/abstract/ct_abstractactionforgraphicsview.cpp \
    ct_actions/abstract/ct_abstractactionfortreeview.cpp \
    ct_actions/view/abstract/ct_gabstractactionforgraphicsviewoptions.cpp

TRANSLATIONS += languages/ctlibaction_fr.ts \
                languages/ctlibaction_en.ts

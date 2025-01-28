QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)

TARGET = ctlibstepaddon

DEFINES += CTLIBSTEPADDON_LIBRARY

HEADERS += \
    ctlibstepaddon_global.h \
    ct_step/abstract/ct_abstractsteploadfileinscene.h \
    ct_step/ct_stepbeginloop.h \
    ct_step/ct_stependloop.h \
    ct_tools/ct_monitoredqthread.h

SOURCES += \
    ct_step/abstract/ct_abstractsteploadfileinscene.cpp \
    ct_step/ct_stepbeginloop.cpp \
    ct_step/ct_stependloop.cpp \
    ct_tools/ct_monitoredqthread.cpp

TRANSLATIONS += languages/ctlibstepaddon_fr.ts \
                languages/ctlibstepaddon_en.ts

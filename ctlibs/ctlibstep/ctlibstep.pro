QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)

TARGET = ctlibstep

DEFINES += CTLIBSTEP_LIBRARY

HEADERS += \
    ct_step/abstract/ct_abstractstep.h \
    ct_step/abstract/ct_abstractsteploadfile.h \
    ct_step/abstract/ct_virtualabstractstep.h \
    ct_step/interfacesforstep.h \
    ct_step/tools/input/ct_inmanager.h \
    ct_step/tools/output/ct_outmanager.h \
    ct_step/view/ct_stepconfigurabledialog.h \
    ct_step/abstract/ct_abstractstepcanbeaddedfirst.h \
    ct_step/tools/ct_uniqueindexgenerator.h \
    ct_step/tools/ct_logmanageradapterforstep.h \
    ctlibstep_global.h

SOURCES += \ 
    ct_step/abstract/ct_abstractsteploadfile.cpp \
    ct_step/abstract/ct_virtualabstractstep.cpp \
    ct_step/tools/input/ct_inmanager.cpp \
    ct_step/tools/output/ct_outmanager.cpp \
    ct_step/tools/ct_uniqueindexgenerator.cpp \
    ct_step/tools/ct_logmanageradapterforstep.cpp

TRANSLATIONS += languages/ctlibstep_fr.ts \
                languages/ctlibstep_en.ts

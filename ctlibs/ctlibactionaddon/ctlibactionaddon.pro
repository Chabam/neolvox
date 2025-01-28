QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../library_shared.pri)
include(../../include_all.pri)

TARGET = ctlibactionaddon

DEFINES += CTLIBACTIONADDON_LIBRARY

HEADERS += \
    ctlibactionaddon_global.h \
    actions/pb_actionselectitemdrawablegv.h \
    views/actions/pb_actionselectitemdrawablegvoptions.h \
    actions/tools/math.h \
    actions/tools/polygonforpicking.h \
    actions/tools/rectangleforpicking.h

SOURCES += \
    actions/pb_actionselectitemdrawablegv.cpp \
    views/actions/pb_actionselectitemdrawablegvoptions.cpp \
    actions/tools/math.cpp \
    actions/tools/polygonforpicking.cpp \
    actions/tools/rectangleforpicking.cpp

FORMS += \
    views/actions/pb_actionselectitemdrawablegvoptions.ui

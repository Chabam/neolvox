QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)
include(../../config/include_dependencies.pri)

TARGET = ctlibstdactions

DEFINES += CTLIBSTDACTIONS_LIBRARY

HEADERS += action/tools/math.h \
    action/tools/polygonforpicking.h \
    action/tools/rectangleforpicking.h \
    action/tools/ct_actionselecttool.h \
    views/actions/ct_actionselectitemdrawablegvoptions.h \
    action/ct_actionselectitemdrawablegv.h \
    action/pb_actionpickitemsinlist.h \
    views/actions/pb_actionpickitemsinlistoptions.h \
    action/pb_actionshowitemdatagv.h

SOURCES += \ 
    action/tools/math.cpp \
    action/tools/polygonforpicking.cpp \
    action/tools/rectangleforpicking.cpp \
    action/tools/ct_actionselecttool.cpp \
    views/actions/ct_actionselectitemdrawablegvoptions.cpp \
    action/ct_actionselectitemdrawablegv.cpp \
    action/pb_actionpickitemsinlist.cpp \
    views/actions/pb_actionpickitemsinlistoptions.cpp \
    action/pb_actionshowitemdatagv.cpp

FORMS += \
    views/actions/ct_actionselectitemdrawablegvoptions.ui \
    views/actions/pb_actionpickitemsinlistoptions.ui

RESOURCES += \
    icons.qrc

TRANSLATIONS += languages/ctlibstdactions_fr.ts \
                languages/ctlibstdactions_en.ts

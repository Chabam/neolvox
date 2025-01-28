QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)

TARGET = ctlibmetrics

DEFINES += CTLIBMETRICS_LIBRARY

HEADERS += ctlibmetrics_global.h \
    ct_metric/abstract/ct_abstractmetric.h \
    ct_metric/abstract/ct_abstractmetric_raster.h \
    ct_metric/abstract/ct_abstractmetric_xyz.h \
    ct_metric/points/ct_cloudmetrics.h \
    tools/ct_valueandbool.h \
    ct_metric/abstract/ct_abstractmetricgeneric.h \
    ct_metric/tools/ct_metricoutmodelstructuremanager.h

SOURCES += \
    ct_metric/abstract/ct_abstractmetric.cpp \
    ct_metric/abstract/ct_abstractmetric_raster.cpp \
    ct_metric/abstract/ct_abstractmetric_xyz.cpp \
    ct_metric/points/ct_cloudmetrics.cpp \
    ct_metric/abstract/ct_abstractmetricgeneric.cpp


TRANSLATIONS += languages/ctlibmetrics_fr.ts \
                languages/ctlibmetrics_en.ts

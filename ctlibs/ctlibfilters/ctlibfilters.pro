QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

MUST_USE_OPENCV = 1


include(../../config/library_shared.pri)

TARGET = ctlibfilters

DEFINES += CTLIBFILTERS_LIBRARY

HEADERS += ctlibfilters_global.h \
    ct_filter/abstract/ct_abstractfilter.h \
    ct_filter/abstract/ct_abstractfilter_rasterfloat.h \
    ct_filter/abstract/ct_abstractfilter_rasterrgbi.h \
    ct_filter/abstract/ct_abstractfilter_xyz.h

SOURCES += \
    ct_filter/abstract/ct_abstractfilter.cpp \
    ct_filter/abstract/ct_abstractfilter_rasterfloat.cpp \
    ct_filter/abstract/ct_abstractfilter_rasterrgbi.cpp \
    ct_filter/abstract/ct_abstractfilter_xyz.cpp


TRANSLATIONS += languages/ctlibfilters_fr.ts \
                languages/ctlibfilters_en.ts

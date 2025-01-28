QT = core gui

include(../../config/library_shared.pri)

TARGET = ctlibmath

DEFINES += CTLIBMATH_LIBRARY

HEADERS += \
    ct_math/ct_math2dlines.h \
    ct_math/ct_mathboundingshape.h \
    ct_math/ct_mathfittedline2d.h \
    ct_math/ct_mathpoint.h \
    ct_math/ct_mathstatistics.h \
    ctlibmath_global.h

SOURCES += \ 
    ct_math/ct_math2dlines.cpp \
    ct_math/ct_mathboundingshape.cpp \
    ct_math/ct_mathfittedline2d.cpp \
    ct_math/ct_mathpoint.cpp \
    ct_math/ct_mathstatistics.cpp

TRANSLATIONS += languages/ctlibmath_fr.ts \
                languages/ctlibmath_en.ts

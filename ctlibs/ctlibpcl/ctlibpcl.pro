MUST_USE_PCL = 1
MUST_USE_BOOST = 1
MUST_USE_FLANN = 1

include(../../config/library_shared.pri)
include(../../config/include_dependencies.pri)

TARGET = ctlibpcl

DEFINES += CTLIBPCL_LIBRARY

HEADERS += \
    ctlibpcl_global.h \
    readers/ct_reader_pcd.h \
    tools/ct_pcltools.h \
    ct_pcldefines.h

SOURCES += \
    readers/ct_reader_pcd.cpp \
    tools/ct_pcltools.cpp


TRANSLATIONS += languages/ctlibpcl_fr.ts \
                languages/ctlibpcl_en.ts

# c++14 for PCL (for the moment, because of FLANN issues with C++17)
CONFIG -= c++17
CONFIG += c++14

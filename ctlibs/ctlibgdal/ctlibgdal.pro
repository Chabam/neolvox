CT_PREFIX = ../..
CT_PREFIX_INSTALL = ../../..

MUST_USE_GDAL = 1

# need opencv
COMPUTREE += ctlibopencv

include($${CT_PREFIX}/shared.pri)
include($${PLUGIN_SHARED_DIR}/include.pri)
include($${CT_PREFIX}/include_ct_library.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += concurrent

DESTDIR = $${PLUGIN_DESTDIR}
TARGET = ctlibgdal

DEFINES += CTLIBGDAL_LIBRARY

HEADERS += $${PLUGIN_SHARED_INTERFACE_DIR}/interfaces.h \
    tools/ct_gdaltools.h

SOURCES += 

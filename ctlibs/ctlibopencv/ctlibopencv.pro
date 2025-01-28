CT_PREFIX = ../..
CT_PREFIX_INSTALL = ../../..

MUST_USE_OPENCV = 1

include($${CT_PREFIX}/shared.pri)
include($${PLUGIN_SHARED_DIR}/include.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += concurrent

DESTDIR = $${PLUGIN_DESTDIR}
TARGET = ctlibgopencv

DEFINES += CTLIBOPENCV_LIBRARY

HEADERS += $${PLUGIN_SHARED_INTERFACE_DIR}/interfaces.h

SOURCES += 

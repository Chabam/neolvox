#-------------------------------------------------
#
# Project created by QtCreator 2018-11-12T09:21:21
#
#-------------------------------------------------

QT       += testlib

QT       += gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

TARGET = tst_testmodels
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        tst_testmodels.cpp 

DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ..
INCLUDEPATH += ../..

COMPUTREE = ctlibmodels

CT_PREFIX = ../../..
CT_PREFIX_INSTALL = ../../../..
CT_LIB_PREFIX = ../..

include($${CT_PREFIX}/destdir.pri)
include($${CT_PREFIX}/include_ct_library.pri)

!equals(PWD, $${OUT_PWD}) {
    error("Shadow build seems to be activated, please desactivated it !")
}

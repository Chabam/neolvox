#-------------------------------------------------
#
# Project created by QtCreator 2018-11-22T09:30:11
#
#-------------------------------------------------

QT       += testlib

QT       += gui

TARGET = tst_teststructure
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
        tst_teststructure.cpp 

DEFINES += SRCDIR=\\\"$$PWD/\\\"

##### TODO : remove it to use eigen from pcl or from 3rdparty ######

INCLUDEPATH += ../../3rdparty/eigen
TR_EXCLUDE  += ../../3rdparty/eigen/*

INCLUDEPATH += ..
INCLUDEPATH += ../..

COMPUTREE = ctlibstructure

CT_PREFIX = ../../..
CT_PREFIX_INSTALL = ../../../..
CT_LIB_PREFIX = ../..

include($${CT_PREFIX}/destdir.pri)
include($${CT_PREFIX}/include_ct_library.pri)

!equals(PWD, $${OUT_PWD}) {
    error("Shadow build seems to be activated, please desactivated it !")
}

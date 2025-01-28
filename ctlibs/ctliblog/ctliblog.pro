QT = core

include(../../config/library_shared.pri)

TARGET = ctliblog

DEFINES += CTLIBLOG_LIBRARY

HEADERS +=  \
    ../loginterface.h \
    ct_log/ct_fileloglistener.h \
    ct_log/ct_logmanager.h \
    ct_risformat/ct_parseris.h \
    ctliblog_global.h \
    ct_log/ct_temporarylog.h \
    ct_log/abstract/ct_abstractloglistener.h

SOURCES += \
    ct_log/ct_fileloglistener.cpp \
    ct_log/ct_logmanager.cpp \
    ct_log/ct_temporarylog.cpp \
    ct_log/abstract/ct_abstractloglistener.cpp \
    ct_risformat/ct_parseris.cpp

TRANSLATIONS += languages/ctliblog_fr.ts \
                languages/ctliblog_en.ts

QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

MUST_USE_LASZIP = 1
include(../../config/library_shared.pri)
include(../../config/include_dependencies.pri)

QT += concurrent
QT += xml

TARGET = ctliblaz

DEFINES += CTLIBLAZ_LIBRARY

HEADERS += \
    ctliblaz_global.h \
    readers/ct_laszip_reader.h \
    readers/ct_reader_laz.h \
    readers/headers/ct_lazheader.h \
    tools/laz/abstract/ct_abstractlazpointformat.h \
    tools/laz/ct_lazgpspacket.h \
    tools/laz/ct_lazcolorpacket.h \
    tools/laz/ct_laznirpacket.h \
    tools/laz/ct_lazwavepacket.h \
    tools/laz/ct_lazpointformat0.h \
    tools/laz/ct_lazpointformat1.h \
    tools/laz/ct_lazpointformat2.h \
    tools/laz/ct_lazpointformat3.h \
    tools/laz/ct_lazpointformat4.h \
    tools/laz/ct_lazpointformat5.h \
    tools/laz/ct_lazpointformat6.h \
    tools/laz/ct_lazpointformat7.h \
    tools/laz/ct_lazpointformat8.h \
    tools/laz/ct_lazpointformat9.h \
    tools/laz/ct_lazpointformat10.h \
    exporters/ct_exporter_laz.h \
    exporters/private/ct_lazpiecebypieceprivateexporter.h \
    mergers/ct_sameheadermerger_laz.h \
    mergers/ct_sameheaderformatmerger_laz.h

SOURCES += \
    readers/ct_laszip_reader.cpp \
    readers/ct_reader_laz.cpp \
    readers/headers/ct_lazheader.cpp \
    tools/laz/abstract/ct_abstractlazpointformat.cpp \
    tools/laz/ct_lazgpspacket.cpp \
    tools/laz/ct_lazcolorpacket.cpp \
    tools/laz/ct_laznirpacket.cpp \
    tools/laz/ct_lazwavepacket.cpp \
    tools/laz/ct_lazpointformat0.cpp \
    tools/laz/ct_lazpointformat1.cpp \
    tools/laz/ct_lazpointformat2.cpp \
    tools/laz/ct_lazpointformat3.cpp \
    tools/laz/ct_lazpointformat4.cpp \
    tools/laz/ct_lazpointformat5.cpp \
    tools/laz/ct_lazpointformat6.cpp \
    tools/laz/ct_lazpointformat7.cpp \
    tools/laz/ct_lazpointformat8.cpp \
    tools/laz/ct_lazpointformat9.cpp \
    tools/laz/ct_lazpointformat10.cpp \
    exporters/ct_exporter_laz.cpp \
    exporters/private/ct_lazpiecebypieceprivateexporter.cpp \
    mergers/ct_sameheadermerger_laz.cpp \
    mergers/ct_sameheaderformatmerger_laz.cpp


TRANSLATIONS += languages/ctliblaz_fr.ts \
                languages/ctliblaz_en.ts

# c++14 for LASZIP
CONFIG -= c++17
CONFIG += c++14

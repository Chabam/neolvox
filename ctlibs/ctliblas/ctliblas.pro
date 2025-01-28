QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)
include(../../config/include_dependencies.pri)

QT += concurrent
QT += xml

TARGET = ctliblas

DEFINES += CTLIBLAS_LIBRARY

HEADERS += \
    ctliblas_global.h \
    readers/ct_reader_lasv2.h \
    readers/headers/ct_lasheader.h \
    itemdrawable/las/ct_stdlaspointsattributescontainer.h \
    itemdrawable/las/ct_stdlaspointsattributescontainershortcut.h \
    tools/las/abstract/ct_abstractlaspointformat.h \
    tools/las/ct_lascolorpacket.h \
    tools/las/ct_lasdata.h \
    tools/las/ct_lasdefine.h \
    tools/las/ct_lasgpspacket.h \
    tools/las/ct_lasnirpacket.h \
    tools/las/ct_laspointformat0.h \
    tools/las/ct_laspointformat1.h \
    tools/las/ct_laspointformat2.h \
    tools/las/ct_laspointformat3.h \
    tools/las/ct_laspointformat4.h \
    tools/las/ct_laspointformat5.h \
    tools/las/ct_laspointformat6.h \
    tools/las/ct_laspointformat7.h \
    tools/las/ct_laspointformat8.h \
    tools/las/ct_laspointformat9.h \
    tools/las/ct_laspointformat10.h \
    tools/las/ct_laspointinfo.h \
    tools/las/ct_laswavepacket.h \
    exporters/ct_exporter_las.h \
    filters/abstract/ct_abstractfilter_las.h \
    metrics/abstract/ct_abstractmetric_las.h \
    mergers/ct_sameheadermerger_las.h \
    mergers/ct_sameheaderformatmerger_las.h \
    exporters/private/ct_laspiecebypieceprivateexporter.h

SOURCES += \
    readers/ct_reader_lasv2.cpp \
    readers/headers/ct_lasheader.cpp \
    itemdrawable/las/ct_stdlaspointsattributescontainer.cpp \
    itemdrawable/las/ct_stdlaspointsattributescontainershortcut.cpp \
    tools/las/abstract/ct_abstractlaspointformat.cpp \
    tools/las/ct_lascolorpacket.cpp \
    tools/las/ct_lasdata.cpp \
    tools/las/ct_lasgpspacket.cpp \
    tools/las/ct_lasnirpacket.cpp \
    tools/las/ct_laspointformat0.cpp \
    tools/las/ct_laspointformat1.cpp \
    tools/las/ct_laspointformat2.cpp \
    tools/las/ct_laspointformat3.cpp \
    tools/las/ct_laspointformat4.cpp \
    tools/las/ct_laspointformat5.cpp \
    tools/las/ct_laspointformat6.cpp \
    tools/las/ct_laspointformat7.cpp \
    tools/las/ct_laspointformat8.cpp \
    tools/las/ct_laspointformat9.cpp \
    tools/las/ct_laspointformat10.cpp \
    tools/las/ct_laspointinfo.cpp \
    tools/las/ct_laswavepacket.cpp \
    exporters/ct_exporter_las.cpp \
    filters/abstract/ct_abstractfilter_las.cpp \
    metrics/abstract/ct_abstractmetric_las.cpp \
    mergers/ct_sameheadermerger_las.cpp \
    mergers/ct_sameheaderformatmerger_las.cpp \
    exporters/private/ct_laspiecebypieceprivateexporter.cpp

TRANSLATIONS += languages/ctliblas_fr.ts \
                languages/ctliblas_en.ts

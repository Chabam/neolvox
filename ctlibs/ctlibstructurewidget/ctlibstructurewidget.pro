QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)

TARGET = ctlibstructurewidget

DEFINES += CTLIBSTRUCTUREWIDGET_LIBRARY

HEADERS += \
    ctlibstructurewidget_global.h \
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionmodel.h \
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionselectionmodel.h \
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionwidget.h \
    ct_itemdrawable/tools/collection/abstract/ct_abstractitemdrawablecollectionbuilder.h \
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionbuildert.h \
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionbuildert.hpp \
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionhierarchyresult.h \
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionhierarchystep.h

SOURCES += \ 
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionmodel.cpp \
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionselectionmodel.cpp \
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionwidget.cpp \
    ct_itemdrawable/tools/collection/abstract/ct_abstractitemdrawablecollectionbuilder.cpp \
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionhierarchyresult.cpp \
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionhierarchystep.cpp

FORMS += \
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionwidget.ui

TRANSLATIONS += languages/ctlibstructurewidget_fr.ts \
                languages/ctlibstructurewidget_en.ts

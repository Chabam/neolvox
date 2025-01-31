set(CTLIBSTRUCTUREWIDGET_HEADERS
    ctlibstructurewidget_global.h
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionmodel.h
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionselectionmodel.h
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionwidget.h
    ct_itemdrawable/tools/collection/abstract/ct_abstractitemdrawablecollectionbuilder.h
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionbuildert.h
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionbuildert.hpp
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionhierarchyresult.h
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionhierarchystep.h
)
set(CTLIBSTRUCTUREWIDGET_SOURCES
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionmodel.cpp
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionselectionmodel.cpp
    ct_itemdrawable/view/ct_itemdrawablehierarchycollectionwidget.cpp
    ct_itemdrawable/tools/collection/abstract/ct_abstractitemdrawablecollectionbuilder.cpp
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionhierarchyresult.cpp
    ct_itemdrawable/tools/collection/ct_itemdrawablecollectionhierarchystep.cpp
)

set(CTLIBSTRUCTUREWIDGET_TRANSLATIONS
    languages/ctlibstructurewidget_fr.ts
    languages/ctlibstructurewidget_en.ts
)

add_library(ctlibstructurewidget SHARED
    ${CTLIBSTRUCTUREWIDGET_HEADERS}
    ${CTLIBSTRUCTUREWIDGET_SOURCES}
    ${CTLIBSTRUCTUREWIDGET_TRANSLATIONS}
)

target_include_directories(ctlibstructurewidget PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibstructurewidget PRIVATE CTLIBSTRUCTUREWIDGET_LIBRARY)
target_link_libraries(ctlibstructurewidget
    ctlibcommon
    ctlibstructure
    ctlibstep
    Qt5::Core
    Qt5::Widgets
)

project(ctlibfilters)

set(CTLIBFILTERS_HEADERS
    ctlibfilters_global.h
    ct_filter/abstract/ct_abstractfilter.h
    ct_filter/abstract/ct_abstractfilter_rasterfloat.h
    ct_filter/abstract/ct_abstractfilter_rasterrgbi.h
    ct_filter/abstract/ct_abstractfilter_xyz.h
)

set(CTLIBFILTERS_SOURCES
    ct_filter/abstract/ct_abstractfilter.cpp
    ct_filter/abstract/ct_abstractfilter_rasterfloat.cpp
    ct_filter/abstract/ct_abstractfilter_rasterrgbi.cpp
    ct_filter/abstract/ct_abstractfilter_xyz.cpp
)

set(CTLIBFILTERS_TRANSLATIONS
    languages/ctlibfilters_fr.ts
    languages/ctlibfilters_en.ts
)

add_library(ctlibfilters SHARED
    ${CTLIBFILTERS_HEADERS}
    ${CTLIBFILTERS_SOURCES}
    ${CTLIBFILTERS_TRANSLATIONS}
)

target_include_directories(ctlibfilters PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibfilters PRIVATE CTLIBFILTERS_LIBRARY)
target_link_libraries(ctlibfilters
    ctlibcommon
    ctlibwidget
    ctlibstructureaddon
    ctliblog
    opencv_core
    Qt5::Core
    Qt5::Widgets
)

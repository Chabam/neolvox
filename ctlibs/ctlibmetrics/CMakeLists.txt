set(CTLIBMETRICS_HEADERS
    ctlibmetrics_global.h
    ct_metric/abstract/ct_abstractmetric.h
    ct_metric/abstract/ct_abstractmetric_raster.h
    ct_metric/abstract/ct_abstractmetric_xyz.h
    ct_metric/points/ct_cloudmetrics.h
    tools/ct_valueandbool.h
    ct_metric/abstract/ct_abstractmetricgeneric.h
    ct_metric/tools/ct_metricoutmodelstructuremanager.h
)

set(CTLIBMETRICS_SOURCES
    ct_metric/abstract/ct_abstractmetric.cpp
    ct_metric/abstract/ct_abstractmetric_raster.cpp
    ct_metric/abstract/ct_abstractmetric_xyz.cpp
    ct_metric/points/ct_cloudmetrics.cpp
    ct_metric/abstract/ct_abstractmetricgeneric.cpp
)

set(CTLIBMETRICS_TRANSLATIONS
    languages/ctlibmetrics_fr.ts
    languages/ctlibmetrics_en.ts
)

add_library(ctlibmetrics SHARED
    ${CTLIBMETRICS_HEADERS}
    ${CTLIBMETRICS_SOURCES}
    ${CTLIBMETRICS_TRANSLATIONS}
)

target_include_directories(ctlibmetrics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    # TODO: fix the actual include
    ${CMAKE_SOURCE_DIR}/ctlibs
)

target_compile_definitions(ctlibmetrics PRIVATE CTLIBMETRICS_LIBRARY)
target_link_libraries(ctlibmetrics
	ctlibcommon
    ctlibwidget
    ctlibstructureaddon
    ctliblog
	Qt5::Core
	Qt5::Widgets
)

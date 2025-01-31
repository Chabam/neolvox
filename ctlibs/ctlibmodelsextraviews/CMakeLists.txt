project(ctlibmodelsextraviews)

set(CTLIBMODELSEXTRAVIEWS_HEADERS
    ctlibmodelsextraviews_global.h
    ctg_modelslinkconfigurationflowview.h
    tools/ct_modelflowdata.h
    tools/ct_modelflowdatamodel.h
    tools/ct_indatatypetooutdatatypeconverter.h
    tools/ct_helpgraphicsitem.h
)

set(CTLIBMODELSEXTRAVIEWS_SOURCES
    ctg_modelslinkconfigurationflowview.cpp
    tools/ct_modelflowdata.cpp
    tools/ct_modelflowdatamodel.cpp
    tools/ct_indatatypetooutdatatypeconverter.cpp
    tools/ct_helpgraphicsitem.cpp
)

set(CTLIBMODELSEXTRAVIEWS_FORMS ctg_modelslinkconfigurationflowview.ui)
set(CTLIBMODELSEXTRAVIEWS_TRANSLATIONS
    languages/ctlibmodelsextraviews_fr.ts
    languages/ctlibmodelsextraviews_en.ts
)

add_library(ctlibmodelsextraviews SHARED
    ${CTLIBMODELSEXTRAVIEWS_HEADERS}
    ${CTLIBMODELSEXTRAVIEWS_SOURCES}
    ${CTLIBMODELSEXTRAVIEWS_FORMS}
    ${CTLIBMODELSEXTRAVIEWS_TRANSLATIONS}
)

target_include_directories(ctlibmodelsextraviews PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    # TODO: fix the actual include
    ${CMAKE_SOURCE_DIR}/ctlibs
)

target_compile_definitions(ctlibmodelsextraviews PRIVATE CTLIBMODELSEXTRAVIEWS_LIBRARY)
target_link_libraries(ctlibmodelsextraviews
    ctlibcommon
    ctlibmodels
    nodes
    Qt5::Core
    Qt5::Widgets)

set(CTLIBPLUGIN_HEADERS
    ctlibplugin_global.h
    ct_abstractstepplugin.h
    ct_exporter/private/ct_standardexporterseparator_p.h
    ct_reader/ct_standardreaderseparator.h
    ct_exporter/ct_standardexporterseparator.h
    ct_step/tools/menu/ct_menulevel.h
    ct_step/tools/menu/ct_stepsmenu.h
    ct_actions/ct_actionsseparator.h
    ct_global/ct_context.h
    pluginentryinterface.h
)

set(CTLIBPLUGIN_SOURCES
    ct_abstractstepplugin.cpp
    ct_reader/ct_standardreaderseparator.cpp
    ct_exporter/ct_standardexporterseparator.cpp
    ct_step/tools/menu/ct_menulevel.cpp
    ct_step/tools/menu/ct_stepsmenu.cpp
    ct_actions/ct_actionsseparator.cpp
    ct_global/ct_context.cpp
)

set(CTLIBPLUGIN_TRANSLATIONS
    languages/ctlibplugin_fr.ts
    languages/ctlibplugin_en.ts
)

add_library(ctlibplugin SHARED
    ${CTLIBPLUGIN_HEADERS}
    ${CTLIBPLUGIN_SOURCES}
    ${CTLIBPLUGIN_TRANSLATIONS}
)

target_include_directories(ctlibplugin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibplugin PRIVATE CTLIBPLUGIN_LIBRARY)
target_link_libraries(ctlibplugin
    ctlibcommon
    ctlibstep
    ctlibexporter
    ctlibreader
    ctlibaction
    ctlibfilters
    ctlibmetrics
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
)

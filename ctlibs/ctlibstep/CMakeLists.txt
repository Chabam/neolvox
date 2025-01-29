set(CTLIBSTEP_SOURCES
    ct_step/abstract/ct_abstractsteploadfile.cpp
    ct_step/abstract/ct_virtualabstractstep.cpp
    ct_step/tools/input/ct_inmanager.cpp
    ct_step/tools/output/ct_outmanager.cpp
    ct_step/tools/ct_uniqueindexgenerator.cpp
    ct_step/tools/ct_logmanageradapterforstep.cpp
)

set(CTLIBSTEP_TRANSLATIONS
    languages/ctlibstep_fr.ts
    languages/ctlibstep_en.ts
)

add_library(ctlibstep SHARED
    ${CTLIBSTEP_SOURCES}
    ${CTLIBSTEP_TRANSLATIONS}
)

target_include_directories(ctlibstep PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibstep PRIVATE CTLIBSTEP_LIBRARY)
target_link_libraries(ctlibstep
    ctlibcommon
    ctlibstructure
    ctlibwidget
    ctliblog
    ctlibmodelsextraviews
    Qt5::Core
    Qt5::Widgets
)

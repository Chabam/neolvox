set(CTLIBSTEPADDON_HEADERS
    ctlibstepaddon_global.h
    ct_step/abstract/ct_abstractsteploadfileinscene.h
    ct_step/ct_stepbeginloop.h
    ct_step/ct_stependloop.h
    ct_tools/ct_monitoredqthread.h
)

set(CTLIBSTEPADDON_SOURCES
    ct_step/abstract/ct_abstractsteploadfileinscene.cpp
    ct_step/ct_stepbeginloop.cpp
    ct_step/ct_stependloop.cpp
    ct_tools/ct_monitoredqthread.cpp
)

set(CTLIBSTEPADDON_TRANSLATIONS
    languages/ctlibstepaddon_fr.ts
    languages/ctlibstepaddon_en.ts
)

add_library(ctlibstepaddon SHARED
    ${CTLIBSTEPADDON_HEADERS}
    ${CTLIBSTEPADDON_SOURCES}
    ${CTLIBSTEPADDON_FORMS}
    ${CTLIBSTEPADDON_TRANSLATIONS}
)

target_include_directories(ctlibstepaddon PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibstepaddon PRIVATE CTLIBSTEPADDON_LIBRARY)
target_link_libraries(ctlibstepaddon
    ctlibcommon
    ctlibstep
    ctlibstructureaddon
    Qt5::Core
    Qt5::Widgets
)

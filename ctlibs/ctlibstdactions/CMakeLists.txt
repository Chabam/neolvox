set(CTLIBSTDACTIONS_SOURCES
    action/ct_actionselectitemdrawablegv.cpp
    action/pb_actionpickitemsinlist.cpp
    action/pb_actionshowitemdatagv.cpp
    action/tools/ct_actionselecttool.cpp
    action/tools/math.cpp
    action/tools/polygonforpicking.cpp
    action/tools/rectangleforpicking.cpp
    views/actions/ct_actionselectitemdrawablegvoptions.cpp
    views/actions/pb_actionpickitemsinlistoptions.cpp
)

set(CTLIBSTDACTIONS_RESOURCES
    icons.qrc
)

set(CTLIBSTDACTIONS_FORMS
    views/actions/ct_actionselectitemdrawablegvoptions.ui
    views/actions/pb_actionpickitemsinlistoptions.ui
)

set(CTLIBSTDACTIONS_TRANSLATIONS
    languages/ctlibstdactions_fr.ts
    languages/ctlibstdactions_en.ts
)

add_library(ctlibstdactions SHARED
    ${CTLIBSTDACTIONS_SOURCES}
    ${CTLIBSTDACTIONS_FORMS}
    ${CTLIBSTDACTIONS_RESOURCES}
    ${CTLIBSTDACTIONS_TRANSLATIONS}
)

target_include_directories(ctlibstdactions PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    # TODO: fix the actual include
    ${CMAKE_SOURCE_DIR}/ctlibs
)

target_compile_definitions(ctlibstdactions PRIVATE CTLIBSTDACTIONS_LIBRARY)
target_link_libraries(ctlibstdactions
    ctlibcommon
    ctlibaction
    ctlibstructure
    ctlibclouds
    ctliblog
    ctlibmath
    Qt5::Core
    Qt5::Widgets
)

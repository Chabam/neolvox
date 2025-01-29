set(CTLIBACTION_SOURCES
    ct_actions/abstract/ct_abstractaction.cpp
    ct_actions/view/abstract/ct_gabstractactionoptions.cpp
    ct_actions/abstract/ct_abstractactionforgraphicsview.cpp
    ct_actions/abstract/ct_abstractactionfortreeview.cpp
    ct_actions/view/abstract/ct_gabstractactionforgraphicsviewoptions.cpp
)

set(CTLIBACTION_TRANSLATIONS
    languages/ctlibaction_fr.ts
    languages/ctlibaction_en.ts
)

add_library(ctlibaction SHARED
    ${CTLIBACTION_SOURCES}
    ${CTLIBACTION_TRANSLATIONS}
)

target_include_directories(ctlibaction PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibaction PRIVATE CTLIBACTION_LIBRARY)
target_link_libraries(ctlibaction
    Eigen3::Eigen
    ctlibcommon
    Qt5::Core
    Qt5::Widgets
)

project(ctlibmath)

set(CTLIBMATH_HEADERS
    ct_math/ct_math2dlines.h
    ct_math/ct_mathboundingshape.h
    ct_math/ct_mathfittedline2d.h
    ct_math/ct_mathpoint.h
    ct_math/ct_mathstatistics.h
    ctlibmath_global.h
)

set(CTLIBMATH_SOURCES
    ct_math/ct_math2dlines.cpp
    ct_math/ct_mathboundingshape.cpp
    ct_math/ct_mathfittedline2d.cpp
    ct_math/ct_mathpoint.cpp
    ct_math/ct_mathstatistics.cpp
)

set(CTLIBMATH_TRANSLATIONS
    languages/ctlibmath_fr.ts
    languages/ctlibmath_en.ts
)

add_library(ctlibmath SHARED
    ${CTLIBMATH_HEADERS}
    ${CTLIBMATH_SOURCES}
    ${CTLIBMATH_TRANSLATIONS}
)

target_include_directories(ctlibmath PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibmath PRIVATE CTLIBMATH_LIBRARY)
target_link_libraries(ctlibmath
     ctlibcommon
     Eigen3::Eigen
     Qt5::Core
     Qt5::Widgets
)

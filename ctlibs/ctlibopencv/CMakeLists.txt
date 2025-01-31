set(CTLIBOPENCV_HEADERS
    ctlibopencv_global.h
)

add_library(ctlibopencv INTERFACE
    ${CTLIBOPENCV_HEADERS}
)

target_include_directories(ctlibopencv INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibopencv INTERFACE CTLIBMODELS_LIBRARY)

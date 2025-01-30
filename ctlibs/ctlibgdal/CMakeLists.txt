add_library(ctlibgdal INTERFACE)

target_include_directories(ctlibgdal INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibgdal INTERFACE CTLIBGDAL_LIBRARY)
target_link_libraries(ctlibgdal
    INTERFACE
        ctlibopencv
)

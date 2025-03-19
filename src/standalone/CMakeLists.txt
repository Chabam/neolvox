add_executable(lvox)

target_sources(lvox
    PRIVATE
    main.cpp
)

target_link_libraries(lvox PRIVATE pdalcpp)

set_target_properties(lvox
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

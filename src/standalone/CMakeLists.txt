add_executable(lvox)

target_sources(lvox
    PRIVATE
    main.cpp
)

target_link_libraries(lvox ${PDAL_LIBS})

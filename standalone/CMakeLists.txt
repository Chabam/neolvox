set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lvox)

target_sources(lvox
  PRIVATE
  main.cpp
)

target_link_libraries(lvox PUBLIC lvox::core)

set_target_properties(lvox
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)


if (WIN32)
  target_compile_definitions(lvox PRIVATE NOMINMAX)

  install(FILES
    $<TARGET_RUNTIME_DLLS:lvox>
    DESTINATION bin
  )
endif()

include(InstallRequiredSystemLibraries)
include(GNUInstallDirs)

set(CMAKE_INSTALL_RPATH $ORIGIN $ORIGIN/${CMAKE_INSTALL_BINDIR})

set(CMAKE_INSTALL_LIBDIR lib)
set(CPACK_PACKAGE_NAME "Neolvox")
set(CPACK_PACKAGE_VENDOR "Félix Chabot")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An application that computes PAD for lidar scans in forests")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

include(CPack)

cpack_add_component(lvox DEPENDS lvox::core)

install(TARGETS
  lvox
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# NOTE: The general tactic here to reduce code duplication is to use
# cmake to copy the required files to this folder and then use the
# standard R packaging commands on it. It is best pratice for R
# extensions to include only source files, we are basically just using
# cmake as a glorified bash script! The neat thing is that you can use
# target properties to retreive the relevant source files.

# TODO: Investigate if cmake could also be used to build the package
# using R commands? Not sure this is a good idea, feels a bit cursed.

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION.in
  ${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION
  @ONLY
)

get_target_property(LVOX_CORE_INCLUDE_DIR lvox::core INCLUDE_DIRECTORIES)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  file(COPY
    "${LVOX_CORE_INCLUDE_DIR}"
    DESTINATION
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
  )
else()
  file(CREATE_LINK
    "${LVOX_CORE_INCLUDE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
    SYMBOLIC
  )

endif()

get_target_property(LVOX_CORE_SOURCES lvox::core SOURCES)
get_target_property(LVOX_CORE_SOURCE_DIR lvox::core SOURCE_DIR)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  foreach(path ${LVOX_CORE_SOURCES})
    file(COPY
      "${LVOX_CORE_SOURCE_DIR}/${path}"
      DESTINATION
      "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
  endforeach()
else()
  foreach(path ${LVOX_CORE_SOURCES})
    get_filename_component(filename ${path} NAME)
    file(CREATE_LINK
      "${LVOX_CORE_SOURCE_DIR}/${path}"
      "${CMAKE_CURRENT_SOURCE_DIR}/src/${filename}"
      SYMBOLIC
    )
  endforeach()

endif()


execute_process(COMMAND R -e "Rcpp::compileAttributes('${CMAKE_CURRENT_SOURCE_DIR}')")
execute_process(COMMAND R -e "roxygen2::roxygenise('${CMAKE_CURRENT_SOURCE_DIR}')")

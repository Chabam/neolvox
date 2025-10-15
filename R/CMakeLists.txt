# NOTE: The general tactic here to reduce code duplication is to use
# cmake to copy the required files to this folder and then use the
# standard R packaging commands on it. It is best pratice for R
# extensions to include only source files, we are basically just using
# cmake as a glorified bash script! The neat thing is that you can use
# target properties to retreive the relevant source files.

# TODO: Investigate if cmake could also be used to build the package
# using R commands? Not sure this is a good idea, feels a bit cursed.


get_target_property(LVOX_CORE_INCLUDE_DIR lvox::core INCLUDE_DIRECTORIES)

file(COPY
  "${LVOX_CORE_INCLUDE_DIR}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

get_target_property(LVOX_CORE_SOURCES lvox::core SOURCES)
get_target_property(LVOX_CORE_SOURCE_DIR lvox::core SOURCE_DIR)

foreach(path ${LVOX_CORE_SOURCES})
  file(COPY
    "${LVOX_CORE_SOURCE_DIR}/${path}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
  )
endforeach()

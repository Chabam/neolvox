MESSAGE(STATUS "Looking for LASzip")

find_path(LASzip_INCLUDE_DIRS
  NAMES laszip.hpp laszip_api.h lazip_api_version.h
  PATH_SUFFIXES laszip
  PATHS
    /include
    /usr/include
    /usr/local/include
  NO_DEFAULT_PATH)

if(UNIX)
  find_library(LASzip_SHARED_LIBRARY
    NAMES laszip laszip3
    PATHS
      /lib
      /usr/lib
      /usr/local/lib)
elseif(WIN32)
  find_library(LASzip_SHARED_LIBRARY
    NAMES laszip laszip3
    PATHS
      /bin)
else()
  message(FATAL_ERROR Unsupported platform)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LASzip LASzip_SHARED_LIBRARY LASzip_INCLUDE_DIRS)

add_library(LASzip SHARED IMPORTED)
target_include_directories(LASzip INTERFACE ${LASzip_INCLUDE_DIRS})
set_target_properties(LASzip PROPERTIES
  IMPORTED_LOCATION ${LASzip_SHARED_LIBRARY}
  INTERFACE_INCLUDE_DIRECTORIES ${LASzip_INCLUDE_DIRS}
)

if (WIN32)
  find_library(LASzip_IMPLIB
    NAMES laszip laszip3
    PATHS
      /bin
      /usr/bin
      /usr/local/bin)

  set_target_property(LASzip APPEND PROPERTY
    IMPORTED_IMPLIB ${LASzip_IMPLIB}
  )
endif()

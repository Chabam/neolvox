MESSAGE(STATUS "Looking for LASzip")

find_path(LASzip_INCLUDE_DIRS
  NAMES laszip.hpp laszip_api.h lazip_api_version.h
  PATH_SUFFIXES laszip
  PATHS
    /include
    /usr/include
    /usr/local/include
  NO_DEFAULT_PATH)


find_library(LASzip_LIBRARY
  NAMES laszip laszip3
  PATHS
    /lib
    /usr/lib
    /usr/local/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LASzip LASzip_LIBRARY LASzip_INCLUDE_DIRS)

add_library(LASzip SHARED IMPORTED)
target_include_directories(LASzip INTERFACE ${LASzip_INCLUDE_DIRS})
set_property(TARGET LASzip PROPERTY
  IMPORTED_LOCATION ${LASzip_LIBRARY}
)

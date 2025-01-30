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

  find_package_handle_standard_args(LASzip LASzip_LIBRARY LASzip_INCLUDE_DIRS)

MESSAGE(STATUS "Looking for LASzip")

find_path(LASzip_INCLUDE_DIRS
  NAMES laszip.hpp laszip_api.h lazip_api_version.h
  PATH_SUFFIXES laszip
  PATHS
    /include
    /usr/include
    /usr/local/include
  NO_DEFAULT_PATH)


find_library(LASzip_RELEASE_LIBRARY
  NAMES laszip laszip3
  PATHS
    /lib
    /usr/lib
    /usr/local/lib)

find_library(LASzip_DEBUG_LIBRARY
  NAMES laszip laszip3
  PATHS
    debug/lib
    debug/usr/lib
    debug/usr/local/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LASzip LASzip_LIBRARY LASzip_INCLUDE_DIRS)

add_library(LASzip SHARED IMPORTED)
target_include_directories(LASzip INTERFACE ${LASzip_INCLUDE_DIRS})
set_property(TARGET LASzip PROPERTY
  IMPORTED_LOCATION ${LASzip_RELEASE_LIBRARY}
)

if(laszip_DEBUG_LIBRARY)
  set_property(TARGET LASzip PROPERTY
    IMPORTED_LOCATION_DEBUG ${LASzip_RELEASE_LIBRARY}
  )
endif()


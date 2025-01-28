# NOTE: using find_package for the moment, may be fallback to
# fetchContent in the future?

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/cmake/modules")

# Qt settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS REQUIRED Widgets Core Gui)

find_package(Eigen3 REQUIRED)
find_package(GDAL REQUIRED)
find_package(muparser REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PCL REQUIRED)
find_package(Boost REQUIRED)
find_package(Qhull REQUIRED)
find_package(flann REQUIRED)
find_package(GSL REQUIRED)
find_package(LASzip REQUIRED)

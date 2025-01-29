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

include(FetchContent)

# NOTE: The original authors of Computree did some changes to this fork...
set(NodeEditorPatch git apply ${CMAKE_SOURCE_DIR}/external/computree-custom.patch)
FetchContent_Declare(NodeEditor
    GIT_REPOSITORY https://github.com/Daguerreo/NodeEditor
    GIT_TAG 9bf1549e9bf1f4bf75c2a381402593c4a8b21a27
    PATCH_COMMAND ${NodeEditorPatch}
    UPDATE_DISCONNECTED 1
)
FetchContent_MakeAvailable(NodeEditor)
find_package(nodes)

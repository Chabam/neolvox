# NOTE: using find_package for the moment, may be fallback to
# fetchContent in the future?

# Qt settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS REQUIRED Widgets Core Gui Xml)

find_package(muparser REQUIRED)

if (PROJECT_IS_TOP_LEVEL)
    include(FetchContent)

    FetchContent_Declare(ctlibs
        GIT_REPOSITORY https://github.com/Chabam/computreelibs
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(ctlibs)
endif()

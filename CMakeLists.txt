cmake_minimum_required(VERSION 3.26)

project(neolvox VERSION 0.0.11 LANGUAGES CXX)

option("LVOX_COMPUTREE_BINDINGS" "Enables binding with the Computree's platform" OFF)
option("LVOX_STANDALONE" "Enables building LVOX as a standalone executable" ON)
option("LVOX_R" "Enables the packaging of LVOX as a R package " OFF)
option("BUILD_TESTING" "Enables the unit tests" OFF)

if (!PROJECT_IS_TOP_LEVEL)
  set(LVOX_COMPUTREE_BINDINGS ON)
  set(LVOX_STANDALONE OFF)
endif()

if (BUILD_TESTING)
    enable_testing()
endif()

add_subdirectory(core)

if (LVOX_COMPUTREE_BINDINGS)
    add_subdirectory(computree/lvox)
endif()

if (LVOX_STANDALONE)
    add_subdirectory(standalone)
endif()

if (LVOX_R)
    add_subdirectory(R)
endif()

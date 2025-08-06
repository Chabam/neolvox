cmake_minimum_required(VERSION 3.26)

project(neolvox VERSION 0.0.0 LANGUAGES CXX)

option("LVOX_COMPUTREE_BINDINGS" "Enables binding with the Computree's platform" ON)
option("LVOX_STANDALONE" "Enables building LVOX as a standalone executable" OFF)
option("BUILD_TESTING" "Enables the unit tests" OFF)

if (!PROJECT_IS_TOP_LEVEL)
  set(LVOX_COMPUTREE_BINDINGS ON)
  set(LVOX_STANDALONE OFF)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/external/dependencies.cmake)

if (BUILD_TESTING)
    enable_testing()
endif()

add_subdirectory(src)

set(CTLIBPCL_SOURCES
    readers/ct_reader_pcd.cpp
    tools/ct_pcltools.cpp
)

set(CTLIBPCL_TRANSLATIONS
    languages/ctlibpcl_fr.ts
    languages/ctlibpcl_en.ts
)

add_library(ctlibpcl SHARED
    ${CTLIBPCL_SOURCES}
    ${CTLIBPCL_TRANSLATIONS}
)

target_include_directories(ctlibpcl PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibpcl PRIVATE CTLIBPCL_LIBRARY)
target_link_libraries(ctlibpcl
	ctlibcommon
    ctlibplugin
    ctliblog
    ctlibclouds
    ctlibstructureaddon
    ctlibreader
	Qt5::Core
	Qt5::Widgets
    pcl_common
)

set(CTLIBREADER_SOURCES
    ct_itemdrawable/ct_indexablepointfileheader.cpp
    ct_reader/abstract/ct_abstractreader.cpp
    ct_itemdrawable/ct_fileheader.cpp
    ct_reader/extensions/ct_indexablepointsreader.cpp
    ct_reader/tools/ct_readeroutmodelstructuremanager.cpp
    ct_reader/extensions/ct_readerpointsfilteringextension.cpp
)

set(CTLIBREADER_TRANSLATIONS
    languages/ctlibreader_fr.ts
    languages/ctlibreader_en.ts
)

add_library(ctlibreader SHARED
    ${CTLIBREADER_SOURCES}
    ${CTLIBREADER_TRANSLATIONS}
)

target_include_directories(ctlibreader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibreader PRIVATE CTLIBREADER_LIBRARY)
target_link_libraries(ctlibreader
	ctlibcommon
    ctlibstructure
    ctlibstructureaddon
    ctlibwidget
	Qt5::Core
	Qt5::Widgets
)

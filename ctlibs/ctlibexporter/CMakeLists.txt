set(CTLIBEXPORTER_HEADERS
    ct_exporter/abstract/ct_abstractexporter.h
    ctlibexporter_global.h
    ct_exporter/tools/ct_exporterinmodelstructuremanager.h
    ct_exporter/abstract/ct_abstractpiecebypieceexporter.h
)

set(CTLIBEXPORTER_SOURCES
    ct_exporter/abstract/ct_abstractexporter.cpp
    ct_exporter/tools/ct_exporterinmodelstructuremanager.cpp
    ct_exporter/abstract/ct_abstractpiecebypieceexporter.cpp
)

set(CTLIBEXPORTER_TRANSLATIONS
    languages/ctlibexporter_fr.ts
    languages/ctlibexporter_en.ts
)

add_library(ctlibexporter SHARED
    ${CTLIBEXPORTER_HEADERS}
    ${CTLIBEXPORTER_SOURCES}
    ${CTLIBEXPORTER_TRANSLATIONS}
)

target_include_directories(ctlibexporter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibexporter PRIVATE CTLIBEXPORTER_LIBRARY)
target_link_libraries(ctlibexporter
	ctlibcommon
    ctlibwidget
    ctlibstructureaddon
    ctlibmodelsextraviews
    Eigen3::Eigen
	Qt5::Core
	Qt5::Widgets
)

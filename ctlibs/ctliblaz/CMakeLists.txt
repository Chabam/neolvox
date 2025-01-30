set(CTLIBLAZ_SOURCES
    readers/ct_laszip_reader.cpp
    readers/ct_reader_laz.cpp
    readers/headers/ct_lazheader.cpp
    tools/laz/abstract/ct_abstractlazpointformat.cpp
    tools/laz/ct_lazgpspacket.cpp
    tools/laz/ct_lazcolorpacket.cpp
    tools/laz/ct_laznirpacket.cpp
    tools/laz/ct_lazwavepacket.cpp
    tools/laz/ct_lazpointformat0.cpp
    tools/laz/ct_lazpointformat1.cpp
    tools/laz/ct_lazpointformat2.cpp
    tools/laz/ct_lazpointformat3.cpp
    tools/laz/ct_lazpointformat4.cpp
    tools/laz/ct_lazpointformat5.cpp
    tools/laz/ct_lazpointformat6.cpp
    tools/laz/ct_lazpointformat7.cpp
    tools/laz/ct_lazpointformat8.cpp
    tools/laz/ct_lazpointformat9.cpp
    tools/laz/ct_lazpointformat10.cpp
    exporters/ct_exporter_laz.cpp
    exporters/private/ct_lazpiecebypieceprivateexporter.cpp
    mergers/ct_sameheadermerger_laz.cpp
    mergers/ct_sameheaderformatmerger_laz.cpp
)

set(CTLIBLAZ_TRANSLATIONS
    languages/ctliblaz_fr.ts
    languages/ctliblaz_en.ts
)

add_library(ctliblaz SHARED
    ${CTLIBLAZ_SOURCES}
    ${CTLIBLAZ_TRANSLATIONS}
)

target_include_directories(ctliblaz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctliblaz PRIVATE CTLIBLAZ_LIBRARY)
target_link_libraries(ctliblaz
	ctlibcommon
    ctlibreader
    ctlibexporter
    ctlibstructureaddon
    ctliblas
	Qt5::Core
	Qt5::Widgets
)

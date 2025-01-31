project(ctliblog)

set(CTLIBLOG_HEADERS
    ct_log/ct_fileloglistener.h
    ct_log/ct_logmanager.h
    ct_risformat/ct_parseris.h
    ctliblog_global.h
    ct_log/ct_temporarylog.h
    ct_log/abstract/ct_abstractloglistener.h
)

set(CTLIBLOG_SOURCES
    ct_log/ct_fileloglistener.cpp
    ct_log/ct_logmanager.cpp
    ct_log/ct_temporarylog.cpp
    ct_log/abstract/ct_abstractloglistener.cpp
    ct_risformat/ct_parseris.cpp
)

set(CTLIBLOG_TRANSLATIONS
    languages/ctliblog_fr.ts
    languages/ctliblog_en.ts
)

add_library(ctliblog SHARED
    ${CTLIBLOG_HEADERS}
    ${CTLIBLOG_SOURCES}
    ${CTLIBLOG_TRANSLATIONS}
)

target_include_directories(ctliblog PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctliblog PRIVATE CTLIBLOG_LIBRARY)
target_link_libraries(ctliblog
	ctlibcommon
	Qt5::Core
)

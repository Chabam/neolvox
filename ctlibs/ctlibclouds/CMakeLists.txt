set(CTLIBMODELS_SOURCES
    ct_accessor/ct_edgeaccessor.cpp
    ct_accessor/ct_faceaccessor.cpp
    ct_accessor/ct_pointaccessor.cpp
    ct_accessor/ct_repositoryaccess.cpp
    ct_attributes/abstract/ct_abstractattributesscalar.cpp
    ct_cloud/abstract/ct_abstractcloud.cpp
    ct_cloud/ct_bitcloud.c
    ct_cloud/registered/abstract/ct_abstractcloudregistered.cpp
    ct_cloud/tools/abstract/ct_abstractcloudsynctoglobalcloudmanager.cpp
    ct_cloud/tools/abstract/ct_abstractglobalcloudmanager.cpp
    ct_cloud/tools/ct_cloudsynctoglobalcloudmanager.cpp
    ct_cloud/tools/ct_globaledgecloudmanager.cpp
    ct_cloud/tools/ct_globalfacecloudmanager.cpp
    ct_cloud/tools/ct_globalpointcloudmanager.cpp
    ct_cloud/tools/iglobalcloudlistener.cpp
    ct_cloudindex/abstract/ct_abstractcloudindex.cpp
    ct_cloudindex/registered/abstract/ct_abstractcloudindexregistered.cpp
    ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanager.cpp
    ct_colorcloud/abstract/ct_abstractcolorcloud.cpp
    ct_colorcloud/abstract/ct_abstractmodifiableindexcloudcolormap.cpp
    ct_colorcloud/ct_colorcloudstdvector.cpp
    ct_colorcloud/registered/ct_standardcolorcloudregistered.cpp
    ct_coordinates/ct_defaultcoordinatesystem.cpp
    ct_coordinates/tools/ct_coordinatesystemcloudindex.cpp
    ct_coordinates/tools/ct_coordinatesystemmanager.cpp
    ct_global/ct_cloudscontext.cpp
    ct_global/ct_repository.cpp
    ct_global/ct_repositorymanager.cpp
    ct_iterator/ct_mutableindexiterator.cpp
    ct_iterator/ct_mutablepointiterator.cpp
    ct_iterator/ct_pointiterator.cpp
    ct_mesh/ct_edge.cpp
    ct_mesh/ct_face.cpp
    ct_mesh/ct_mesh.cpp
    ct_normal.cpp
    ct_normalcloud/abstract/ct_abstractnormalcloud.cpp
    ct_normalcloud/ct_normalcloudstdvector.cpp
    ct_normalcloud/registered/ct_standardnormalcloudregistered.cpp
    ct_normalcloud/tools/normalsestimator.cpp
    ct_point.cpp
    ct_pointcloud/ct_internalpointcloud.cpp
    ct_pointcloud/tools/abstract/ct_abstractundefinedsizepointcloud.cpp
    ct_pointcloud/tools/ct_standardundefinedsizepointcloud.cpp
)

set(CTLIBCLOUDS_TRANSLATIONS
    languages/ctlibclouds_fr.ts
    languages/ctlibclouds_en.ts
)

add_library(ctlibclouds SHARED
    ${CTLIBCLOUDS_SOURCES}
    ${CTLIBCLOUDS_TRANSLATIONS}
)

target_include_directories(ctlibclouds PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibclouds PRIVATE CTLIBCLOUDS_LIBRARY)
target_link_libraries(ctlibclouds
	ctlibcommon
	Qt5::Core
    Qt5::Gui
	Qt5::Widgets
)

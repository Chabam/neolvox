QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)

TARGET = ctlibclouds

DEFINES += CTLIBCLOUDS_LIBRARY

HEADERS += \
    ct_defines.h \
    ctlibclouds_global.h \
    ct_accessibility/ct_iaccessedgecloud.h \
    ct_accessibility/ct_iaccessfacecloud.h \
    ct_accessibility/ct_iaccesspointcloud.h \
    ct_normal.h \
    ct_point.h \
    ct_color.h \
    ct_cloud/abstract/ct_abstractcloud.h \
    ct_cloud/abstract/ct_abstractcloudt.h \
    ct_cloud/registered/abstract/ct_abstractcloudregistered.h \
    ct_cloud/registered/ct_stdcloudregisteredt.h \
    ct_cloud/registered/ct_stdcloudregisteredt.hpp \
    ct_cloud/tools/abstract/ct_abstractcloudsynctoglobalcloudmanager.h \
    ct_cloud/tools/abstract/ct_abstractglobalcloudmanager.h \
    ct_cloud/tools/abstract/ct_abstractglobalcloudmanagert.h \
    ct_cloud/tools/abstract/ct_abstractglobalcloudmanagert.hpp \
    ct_cloud/tools/ct_cloudsynctoglobalcloudmanager.h \
    ct_cloud/tools/ct_globalcloudmanagert.h \
    ct_cloud/tools/ct_globalcloudmanagert.hpp \
    ct_cloud/tools/ct_globaledgecloudmanager.h \
    ct_cloud/tools/ct_globalfacecloudmanager.h \
    ct_cloud/tools/ct_globalpointcloudmanager.h \
    ct_cloud/tools/iglobalcloudlistener.h \
    ct_cloudindex/abstract/ct_abstractcloudindex.h \
    ct_cloudindex/abstract/ct_abstractcloudindext.h \
    ct_cloudindex/abstract/ct_abstractmodifiablecloudindex.h \
    ct_cloudindex/abstract/ct_abstractmodifiablecloudindext.h \
    ct_cloudindex/registered/abstract/ct_abstractcloudindexregistered.h \
    ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h \
    ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.hpp \
    ct_cloudindex/registered/abstract/ct_abstractmodifiablecloudindexregistered.h \
    ct_cloudindex/registered/abstract/ct_abstractmodifiablecloudindexregisteredt.h \
    ct_cloudindex/registered/abstract/ct_abstractmodifiablecloudindexregisteredt.hpp \
    ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.h \
    ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.hpp \
    ct_cloudindex/registered/ct_standardmodifiablecloudindexregisteredt.h \
    ct_cloudindex/registered/ct_standardmodifiablecloudindexregisteredt.hpp \
    ct_cloudindex/registered/ct_standardnotmodifiablecloudindexregisteredt.h \
    ct_cloudindex/registered/ct_standardnotmodifiablecloudindexregisteredt.hpp \ \
    ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanager.h \
    ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanagert.h \
    ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanagert.hpp \
    ct_cloudindex/tools/ct_cloudindexiteratort.h \
    ct_cloudindex/tools/ct_cloudindexregistrationmanagert.h \
    ct_cloudindex/tools/ct_cloudindexregistrationmanagert.hpp \
    ct_cloudindex/tools/ct_cloudindexstdvectortmethodimpl.h \
    ct_cloudindex/tools/ct_cloudindexstdvectortmethodimpl.hpp \
    ct_cloudindex/tools/ct_modifiablecloudindexiteratort.h \
    ct_coordinates/abstract/ct_abstractcoordinatesystem.h \
    ct_coordinates/tools/ct_coordinatesystemcloudindex.h \
    ct_coordinates/tools/ct_coordinatesystemmanager.h \
    ct_coordinates/ct_defaultcoordinatesystem.h \
    ct_global/ct_repository.h \
    ct_global/ct_repositorymanager.h \
    ct_pointcloud/abstract/ct_abstractpointcloud.h \
    ct_pointcloud/tools/abstract/ct_abstractundefinedsizepointcloud.h \
    ct_pointcloud/tools/ct_standardundefinedsizepointcloud.h \
    ct_pointcloud/ct_internalpointcloud.h \
    ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h \
    ct_pointcloudindex/ct_pointcloudindexlessmemory.h \
    ct_pointcloudindex/ct_pointcloudindexvector.h \
    ct_accessor/ct_edgeaccessor.h \
    ct_accessor/ct_faceaccessor.h \
    ct_accessor/ct_pointaccessor.h \
    ct_cloud/ct_standardcloudstdvectort.h \
    ct_cloud/ct_standardcloudstdvectort.hpp \
    ct_cloudindex/ct_cloudindexlessmemoryt.h \
    ct_cloudindex/ct_cloudindexlessmemoryt.hpp \
    ct_cloudindex/ct_cloudindexstdlistt.h \
    ct_cloudindex/ct_cloudindexstdlistt.hpp \
    ct_cloudindex/ct_cloudindexstdmapt.h \
    ct_cloudindex/ct_cloudindexstdmapt.hpp \
    ct_cloudindex/ct_cloudindexstdvectort.h \
    ct_cloudindex/ct_cloudindexstdvectort.hpp \
    ct_iterator/ct_edgeiterator.h \
    ct_iterator/ct_faceiterator.h \
    ct_iterator/ct_mutableedgeindexiterator.h \
    ct_iterator/ct_mutableedgeiterator.h \
    ct_iterator/ct_mutablefaceindexiterator.h \
    ct_iterator/ct_mutablefaceiterator.h \
    ct_iterator/ct_mutableindexiterator.h \
    ct_iterator/ct_mutablepointindexiterator.h \
    ct_iterator/ct_mutablepointiterator.h \
    ct_iterator/ct_pointiterator.h \
    ct_mesh/tools/ct_meshallocator.h \
    ct_mesh/tools/ct_meshallocatort.h \
    ct_mesh/tools/ct_meshallocatort.hpp \
    ct_mesh/ct_edge.h \
    ct_mesh/ct_face.h \
    ct_mesh/ct_mesh.h \
    ct_facecloud/abstract/ct_abstractfacecloud.h \
    ct_edgecloud/abstract/ct_abstractedgecloud.h \
    ct_colorcloud/abstract/ct_abstractcolorcloud.h \
    ct_colorcloud/abstract/ct_abstractmodifiableindexcloudcolormap.h \
    ct_colorcloud/registered/ct_standardcolorcloudregistered.h \
    ct_colorcloud/ct_colorcloudstdvector.h \
    ct_colorcloud/ct_indexcloudcolorstdmapt.h \
    ct_colorcloud/ct_indexcloudcolorstdmapt.hpp \
    ct_normalcloud/abstract/ct_abstractnormalcloud.h \
    ct_normalcloud/registered/ct_standardnormalcloudregistered.h \
    ct_normalcloud/ct_normalcloudstdvector.h \
    ct_global/ct_cloudscontext.h \
    ct_accessor/ct_repositoryaccess.h \
    ct_attributes/abstract/ct_abstractattributesscalar.h \
    ct_attributes/ct_attributescolor.h \
    ct_attributes/ct_attributesnormal.h \
    ct_attributes/ct_attributesscalart.h \
    ct_attributes/ct_attributesscalart.hpp \
    ct_normalcloud/tools/iestimatorobserverandcontroller.h \
    ct_normalcloud/tools/normalsestimator.h \
    ct_cloud/ct_bitcloud.h \
    ct_attributes/managers/abstract/ct_abstractxattributemanager.h \
    ct_attributes/managers/ct_denseattributemanager.h \
    ct_attributes/managers/ct_sparseattributemanager.h \
    ct_attributes/managers/ct_denseedgecolormanager.h \
    ct_attributes/managers/ct_denseedgescalarmanager.h \
    ct_attributes/managers/ct_denseedgenormalmanager.h \
    ct_attributes/managers/ct_sparseedgescalarmanager.h \
    ct_attributes/managers/ct_sparseedgecolormanager.h \
    ct_attributes/managers/ct_sparseedgenormalmanager.h \
    ct_attributes/managers/ct_densefacescalarmanager.h \
    ct_attributes/managers/ct_densefacecolormanager.h \
    ct_attributes/managers/ct_densefacenormalmanager.h \
    ct_attributes/managers/ct_sparsefacescalarmanager.h \
    ct_attributes/managers/ct_sparsefacecolormanager.h \
    ct_attributes/managers/ct_sparsefacenormalmanager.h \
    ct_attributes/managers/ct_densepointscalarmanager.h \
    ct_attributes/managers/ct_densepointcolormanager.h \
    ct_attributes/managers/ct_densepointnormalmanager.h \
    ct_attributes/managers/ct_sparsepointscalarmanager.h \
    ct_attributes/managers/ct_sparsepointcolormanager.h \
    ct_attributes/managers/ct_sparsepointnormalmanager.h \
    ct_attributes/setters/ct_denseattributesetter.h \
    ct_attributes/setters/ct_sparseattributesetter.h \
    ct_attributes/setters/ct_undefinedpointcloudsizedenseattributesetter.h \
    ct_attributes/setters/ct_undefinedpointcloudsizesparseattributesetter.h \
    ct_attributes/managers/ct_scalarminmaxmanager.h

SOURCES += \ 
    ct_normal.cpp \
    ct_point.cpp \
    ct_cloud/abstract/ct_abstractcloud.cpp \
    ct_cloud/registered/abstract/ct_abstractcloudregistered.cpp \
    ct_cloud/tools/abstract/ct_abstractcloudsynctoglobalcloudmanager.cpp \
    ct_cloud/tools/abstract/ct_abstractglobalcloudmanager.cpp \
    ct_cloud/tools/ct_cloudsynctoglobalcloudmanager.cpp \
    ct_cloud/tools/ct_globaledgecloudmanager.cpp \
    ct_cloud/tools/ct_globalfacecloudmanager.cpp \
    ct_cloud/tools/ct_globalpointcloudmanager.cpp \
    ct_cloud/tools/iglobalcloudlistener.cpp \
    ct_cloudindex/abstract/ct_abstractcloudindex.cpp \
    ct_cloudindex/registered/abstract/ct_abstractcloudindexregistered.cpp \
    ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanager.cpp \
    ct_coordinates/tools/ct_coordinatesystemcloudindex.cpp \
    ct_coordinates/tools/ct_coordinatesystemmanager.cpp \
    ct_coordinates/ct_defaultcoordinatesystem.cpp \
    ct_global/ct_repository.cpp \
    ct_global/ct_repositorymanager.cpp \
    ct_pointcloud/tools/abstract/ct_abstractundefinedsizepointcloud.cpp \
    ct_pointcloud/tools/ct_standardundefinedsizepointcloud.cpp \
    ct_pointcloud/ct_internalpointcloud.cpp \
    ct_accessor/ct_edgeaccessor.cpp \
    ct_accessor/ct_faceaccessor.cpp \
    ct_accessor/ct_pointaccessor.cpp \
    ct_iterator/ct_mutableindexiterator.cpp \
    ct_iterator/ct_mutablepointiterator.cpp \
    ct_iterator/ct_pointiterator.cpp \
    ct_mesh/ct_edge.cpp \
    ct_mesh/ct_face.cpp \
    ct_mesh/ct_mesh.cpp \
    ct_colorcloud/abstract/ct_abstractcolorcloud.cpp \
    ct_colorcloud/abstract/ct_abstractmodifiableindexcloudcolormap.cpp \
    ct_colorcloud/registered/ct_standardcolorcloudregistered.cpp \
    ct_colorcloud/ct_colorcloudstdvector.cpp \
    ct_normalcloud/abstract/ct_abstractnormalcloud.cpp \
    ct_normalcloud/registered/ct_standardnormalcloudregistered.cpp \
    ct_normalcloud/ct_normalcloudstdvector.cpp \
    ct_global/ct_cloudscontext.cpp \
    ct_accessor/ct_repositoryaccess.cpp \
    ct_attributes/abstract/ct_abstractattributesscalar.cpp \
    ct_normalcloud/tools/normalsestimator.cpp \
    ct_cloud/ct_bitcloud.cpp

TRANSLATIONS += languages/ctlibclouds_fr.ts \
                languages/ctlibclouds_en.ts

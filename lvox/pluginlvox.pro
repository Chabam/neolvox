COMPUTREE += ctlibplugin ctlibclouds ctlibstep ctlibstepaddon ctlibio ctlibfilters ctlibaction ctlibstdactions ctlibmath ctliblas

CHECK_CAN_USE_PCL = 1
CHECK_CAN_USE_GDAL = 1
MUST_USE_OPENCV = 1
MUST_USE_MUPARSER = 1

include(../../computreev6/config/plugin_shared.pri)

QT += concurrent opengl widgets

TARGET = plug_lvoxv3


HEADERS += \
    $$CT_PREFIX_LIB/ctlibplugin/pluginentryinterface.h\
    qtwidgets/multiselectcombobox.h \
    step/lvox3_stepcomputenormalizedprofiles.h \
    step/lvox3_stepmakeshootingpatternformls.h \
    tools/scanner/lvox_shootingpatternformls.h \
    view/computegridsconfiguration.h \
    view/computepadconfiguration.h \
    lvox_steppluginmanager.h \
    lvox_pluginentry.h \
    step/lvox3_stepcomputelvoxgrids.h \
    step/lvox3_steploadfiles.h \
    step/lvox3_stepcomputeprofiles.h \
    step/lvox3_stepinterpolatedistance.h \
    step/lvox3_stepmergegrids.h \
    step/lvox3_stepextractcirculargrid.h \
    step/lvox3_stepcomputepad.h \
    step/lvox3_stepgridnormalisationraster.h \
    tools/worker/lvox3_computehits.h \
    tools/lvox3_errorcode.h \
    tools/worker/lvox3_worker.h \
    tools/worker/lvox3_computetheoriticals.h \
    tools/worker/lvox3_computebefore.h \
    tools/worker/lvox3_computedensity.h \
    tools/lvox3_computelvoxgridspreparator.h \
    tools/lvox3_gridmode.h \
    tools/lvox3_gridtype.h \
    tools/worker/lvox3_computeall.h \
    tools/lvox3_gridtools.h \
    tools/traversal/woo/lvox3_grid3dwootraversalalgorithm.h \
    tools/lvox3_rayboxintersectionmath.h \
    tools/traversal/woo/visitor/lvox3_countvisitor.h \
    tools/traversal/woo/visitor/lvox3_distancevisitor.h \
    view/loadfileconfiguration.h \
    view/genericcomputegridsconfiguration.h \
    view/predefinedmapperconfiguration.h \
    tools/worker/lvox3_genericcompute.h \
    tools/lvox3_genericconfiguration.h \
    view/computeprofilesconfiguration.h \
    tools/worker/lvox3_filtervoxelsbyzvaluesofraster.h \
    tools/worker/lvox3_interpolatedistance.h \
    tools/traversal/propagation/lvox3_grid3dpropagationalgorithm.h \
    tools/traversal/woo/visitor/lvox3_grid3dvoxelwoovisitorcontext.h \
    tools/traversal/woo/visitor/lvox3_grid3dvoxelwoovisitor.h \
    tools/traversal/propagation/visitor/lvox3_propagationvisitor.h \
    tools/traversal/propagation/visitor/lvox3_propagationvisitorcontext.h \
    tools/traversal/propagation/visitor/lvox3_distanceinterpolationvisitor.h \
    tools/traversal/propagation/visitor/lvox3_trustinterpolationvisitor.h \
    tools/worker/lvox3_interpolatetrustfactor.h \
    view/loadfileconfigutil.h \
    tools/lvox3_utils.h \
    view/mergegridsconfiguration.h \
    tools/lvox3_mergegrids.h \
    tools/worker/lvox3_mergegridsworker.h \
    tools/lvox3_factory.hpp \
    tools/lvox3_scannerutils.h \
    tools/3dgrid/abstract/lvox3_abstractgrid3d.h \
    tools/3dgrid/lvox3_grid3d.h \
    tools/3dgrid/lvox3_grid3d.hpp \
    tools/3dgrid/lvox3_grid3d_sparse.h \
    tools/3dgrid/lvox3_grid3d_sparse.hpp \
    tools/drawmanager/lvox3_standardgrid3d_sparsedrawmanager.h \
    tools/drawmanager/lvox3_standardgrid3d_sparsedrawmanager.hpp \
    tools/drawmanager/lvox3_standardgrid3ddrawmanager.hpp \
    tools/drawmanager/lvox3_standardgrid3ddrawmanager.h \
    tools/exporter/lvox3_grid3dexporter.h \
    tools/reader/lvox3_reader_asciigrid3d.h \
    tools/reader/lvox3_reader_asciigrid3d_def_models.h \
    tools/worker/lvox3_interpolatezaverage.h \
    tools/traversal/propagation/expfit.h \
    tools/worker/lvox3_interpolatekrigingbinomial.h \
    tools/worker/lvox3_interpolatekrigingnormal.h \
    tools/lvox3_thetaphishootingpattern.h


SOURCES += \
    qtwidgets/multiselectcombobox.cpp \
    step/lvox3_stepcomputenormalizedprofiles.cpp \
    step/lvox3_stepmakeshootingpatternformls.cpp \
    tools/scanner/lvox_shootingpatternformls.cpp \
    view/computegridsconfiguration.cpp \
    view/computepadconfiguration.cpp \
    lvox_pluginentry.cpp \
    lvox_steppluginmanager.cpp \
    step/lvox3_stepcomputelvoxgrids.cpp \
    step/lvox3_steploadfiles.cpp \
    step/lvox3_stepcomputeprofiles.cpp \
    step/lvox3_stepinterpolatedistance.cpp \
    step/lvox3_stepmergegrids.cpp \
    step/lvox3_stepextractcirculargrid.cpp \
    step/lvox3_stepcomputepad.cpp \
    step/lvox3_stepgridnormalisationraster.cpp \
    tools/worker/lvox3_computehits.cpp \
    tools/worker/lvox3_worker.cpp \
    tools/worker/lvox3_computetheoriticals.cpp \
    tools/worker/lvox3_computebefore.cpp \
    tools/worker/lvox3_computedensity.cpp \
    tools/lvox3_computelvoxgridspreparator.cpp \
    tools/worker/lvox3_computeall.cpp \
    tools/lvox3_rayboxintersectionmath.cpp \
    view/loadfileconfiguration.cpp \
    view/genericcomputegridsconfiguration.cpp \
    view/predefinedmapperconfiguration.cpp \
    tools/worker/lvox3_genericcompute.cpp \
    view/computeprofilesconfiguration.cpp \
    tools/worker/lvox3_filtervoxelsbyzvaluesofraster.cpp \
    tools/worker/lvox3_interpolatedistance.cpp \
    tools/traversal/propagation/lvox3_grid3dpropagationalgorithm.cpp \
    tools/traversal/propagation/visitor/lvox3_distanceinterpolationvisitor.cpp \
    tools/traversal/propagation/visitor/lvox3_trustinterpolationvisitor.cpp \
    tools/worker/lvox3_interpolatetrustfactor.cpp \
    view/loadfileconfigutil.cpp \
    tools/lvox3_scannerutils.cpp \
    tools/lvox3_utils.cpp \
    view/mergegridsconfiguration.cpp \
    tools/lvox3_mergegrids.cpp \
    tools/worker/lvox3_mergegridsworker.cpp \
    tools/3dgrid/abstract/lvox3_abstractgrid3d.cpp \
    tools/3dgrid/lvox3_grid3d.cpp \
    tools/3dgrid/lvox3_grid3d_sparse.cpp \
    tools/drawmanager/lvox3_standardgrid3d_sparsedrawmanager.cpp \
    tools/drawmanager/lvox3_standardgrid3ddrawmanager.cpp \
    tools/exporter/lvox3_grid3dexporter.cpp \
    tools/reader/lvox3_reader_asciigrid3d.cpp \
    tools/worker/lvox3_interpolatezaverage.cpp \
    tools/traversal/propagation/expfit.cpp \
    tools/worker/lvox3_interpolatekrigingbinomial.cpp \
    tools/worker/lvox3_interpolatekrigingnormal.cpp \
    tools/lvox3_thetaphishootingpattern.cpp

TRANSLATIONS += languages/pluginlvoxv2_en.ts \
                languages/pluginlvoxv2_fr.ts

FORMS += \
    view/computegridsconfiguration.ui \
    view/computepadconfiguration.ui \
    view/computeprofilesconfiguration.ui \
    view/genericcomputegridsconfiguration.ui \
    view/loadfileconfiguration.ui \
    view/mergegridsconfiguration.ui \
    view/predefinedmapperconfiguration.ui

RESOURCES += \
    resources.qrc

# make sure changes to headers forces rebuild of the test
DEPENDPATH += $${INCLUDEPATH}


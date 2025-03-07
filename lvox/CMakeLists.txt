set(LVOX_HEADERS
    # TODO: find a better way to handle this...
    # NOTE: We need to explicitly add this header in order
    # for Qt to AUTOMOC it...
    ${ctlibplugin_SOURCE_DIR}/pluginentryinterface.h
    lvox_pluginentry.h
    lvox_steppluginmanager.h
    qtwidgets/multiselectcombobox.h
    step/lvox3_stepcomputelvoxgrids.h
    step/lvox3_stepcomputenormalizedprofiles.h
    step/lvox3_stepcomputepad.h
    step/lvox3_stepcomputeprofiles.h
    step/lvox3_stepextractcirculargrid.h
    step/lvox3_stepgridnormalisationraster.h
    step/lvox3_stepinterpolatedistance.h
    step/lvox3_steploadfiles.h
    step/lvox3_stepmakeshootingpatternformls.h
    step/lvox3_stepmergegrids.h
    tools/3dgrid/lvox_grid3d.h
    tools/3dgrid/lvox_grid3d.hpp
    tools/lvox3_errorcode.h
    tools/lvox3_factory.hpp
    tools/lvox3_genericconfiguration.h
    tools/lvox3_gridmode.h
    tools/lvox3_gridtools.h
    tools/lvox3_gridtype.h
    tools/lvox3_mergegrids.h
    tools/lvox3_rayboxintersectionmath.h
    tools/lvox3_scannerutils.h
    tools/lvox3_utils.h
    tools/scanner/lvox_shootingpatternformls.h
    tools/traversal/propagation/expfit.h
    tools/traversal/propagation/lvox3_grid3dpropagationalgorithm.h
    tools/traversal/propagation/visitor/lvox3_distanceinterpolationvisitor.h
    tools/traversal/propagation/visitor/lvox3_propagationvisitor.h
    tools/traversal/propagation/visitor/lvox3_propagationvisitorcontext.h
    tools/traversal/propagation/visitor/lvox3_trustinterpolationvisitor.h
    tools/traversal/woo/visitor/lvox3_countvisitor.h
    tools/traversal/woo/visitor/lvox3_distancevisitor.h
    tools/worker/lvox3_computeall.h
    tools/worker/lvox3_computebefore.h
    tools/worker/lvox3_computedensity.h
    tools/worker/lvox3_computehits.h
    tools/worker/lvox3_computetheoriticals.h
    tools/worker/lvox3_filtervoxelsbyzvaluesofraster.h
    tools/worker/lvox3_genericcompute.h
    tools/worker/lvox3_interpolatedistance.h
    tools/worker/lvox3_interpolatekrigingbinomial.h
    tools/worker/lvox3_interpolatekrigingnormal.h
    tools/worker/lvox3_interpolatetrustfactor.h
    tools/worker/lvox3_interpolatezaverage.h
    tools/worker/lvox3_mergegridsworker.h
    tools/worker/lvox3_worker.h
    view/computegridsconfiguration.h
    view/computepadconfiguration.h
    view/computeprofilesconfiguration.h
    view/genericcomputegridsconfiguration.h
    view/loadfileconfiguration.h
    view/loadfileconfigutil.h
    view/mergegridsconfiguration.h
    view/predefinedmapperconfiguration.h
)

set(LVOX_SOURCES
    lvox_pluginentry.cpp
    lvox_steppluginmanager.cpp
    qtwidgets/multiselectcombobox.cpp
    step/lvox3_stepcomputelvoxgrids.cpp
    step/lvox3_stepcomputenormalizedprofiles.cpp
    step/lvox3_stepcomputepad.cpp
    step/lvox3_stepcomputeprofiles.cpp
    step/lvox3_stepextractcirculargrid.cpp
    step/lvox3_stepgridnormalisationraster.cpp
    step/lvox3_stepinterpolatedistance.cpp
    step/lvox3_steploadfiles.cpp
    step/lvox3_stepmakeshootingpatternformls.cpp
    step/lvox3_stepmergegrids.cpp
    tools/3dgrid/lvox_grid3d.cpp
    tools/lvox3_mergegrids.cpp
    tools/lvox3_rayboxintersectionmath.cpp
    tools/lvox3_scannerutils.cpp
    tools/lvox3_utils.cpp
    tools/scanner/lvox_shootingpatternformls.cpp
    tools/traversal/propagation/expfit.cpp
    tools/traversal/propagation/lvox3_grid3dpropagationalgorithm.cpp
    tools/traversal/propagation/visitor/lvox3_distanceinterpolationvisitor.cpp
    tools/traversal/propagation/visitor/lvox3_trustinterpolationvisitor.cpp
    tools/worker/lvox3_computeall.cpp
    tools/worker/lvox3_computebefore.cpp
    tools/worker/lvox3_computedensity.cpp
    tools/worker/lvox3_computehits.cpp
    tools/worker/lvox3_computetheoriticals.cpp
    tools/worker/lvox3_filtervoxelsbyzvaluesofraster.cpp
    tools/worker/lvox3_genericcompute.cpp
    tools/worker/lvox3_interpolatedistance.cpp
    tools/worker/lvox3_interpolatekrigingbinomial.cpp
    tools/worker/lvox3_interpolatekrigingnormal.cpp
    tools/worker/lvox3_interpolatetrustfactor.cpp
    tools/worker/lvox3_interpolatezaverage.cpp
    tools/worker/lvox3_mergegridsworker.cpp
    tools/worker/lvox3_worker.cpp
    view/computegridsconfiguration.cpp
    view/computepadconfiguration.cpp
    view/computeprofilesconfiguration.cpp
    view/genericcomputegridsconfiguration.cpp
    view/loadfileconfiguration.cpp
    view/loadfileconfigutil.cpp
    view/mergegridsconfiguration.cpp
    view/predefinedmapperconfiguration.cpp
)

set(LVOX_RESOURCES
    resources.qrc
)

set(LVOX_FORMS
    view/computegridsconfiguration.ui
    view/computepadconfiguration.ui
    view/computeprofilesconfiguration.ui
    view/genericcomputegridsconfiguration.ui
    view/loadfileconfiguration.ui
    view/mergegridsconfiguration.ui
    view/predefinedmapperconfiguration.ui
)

set(LVOX_TRANSLATIONS
    languages/pluginlvoxv2_fr.ts
    languages/pluginlvoxv2_en.ts
)

add_library(plug_neolvox SHARED
    ${LVOX_HEADERS}
    ${LVOX_SOURCES}
    ${LVOX_RESOURCES}
    ${LVOX_FORMS}
    ${LVOX_TRANSLATIONS}
)

target_include_directories(plug_neolvox PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(plug_neolvox PRIVATE
    USE_OPENCV
    $<$<BOOL:${WIN32}>:_USE_MATH_DEFINES>
)

target_link_libraries(plug_neolvox
    ctlibcommon
    ctlibplugin
    ctlibclouds
    ctlibio
    ctlibstep
    ctlibstepaddon
    ctlibio
    ctlibfilters
    ctlibaction
    ctlibstdactions
    ctlibstructure
    ctlibstructureaddon
    ctlibmath
    ctliblas
    muparser::muparser
    Eigen3::Eigen
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

set_target_properties(plug_neolvox
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_command(TARGET plug_neolvox POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:plug_neolvox> $<TARGET_RUNTIME_DLLS:plug_neolvox>
  COMMAND_EXPAND_LISTS
)

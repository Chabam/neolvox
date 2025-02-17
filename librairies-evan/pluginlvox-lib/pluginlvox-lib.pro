COMPUTREE += ctlibplugin ctlibclouds ctlibstep ctlibstepaddon ctlibio ctlibfilters ctlibaction ctlibstdactions ctlibmath ctliblas

CHECK_CAN_USE_PCL = 1
CHECK_CAN_USE_GDAL = 1
MUST_USE_OPENCV = 1
MUST_USE_MUPARSER = 1

include(../../computreev6/config/plugin_shared.pri)

QT += concurrent opengl widgets

TARGET = plug_lvox4

HEADERS += \
    $$CT_PREFIX_LIB/ctlibplugin/pluginentryinterface.h\
    datastructures/lvox3_implgrid3d.h \
    datastructures/lvox3_implpointcloud.h \
    #datastructures/lvox3_implscanner.h \
    datastructures/lvox3_implshootingpattern.h \
    qtwidgets/multiselectcombobox.h \
    step/lvox3_stepcomputenormalizedprofiles.h \
    tools/3dgrid/lvox3_abstractgrid3d.h \
    tools/lvox3_logger.h \
    tools/scanner/lvox_shootingpatternformls.h \
    view/computegridsconfiguration.h \
    view/computepadconfiguration.h \
    lvox_steppluginmanager.h \
    lvox_pluginentry.h \
    step/lvox3_stepcomputelvoxgrids.h \
    step/lvox3_steploadfiles.h \
    step/lvox3_stepinterpolatedistance.h \
    step/lvox3_stepcomputeprofiles.h \
    step/lvox3_stepmergegrids.h \
    step/lvox3_stepextractcirculargrid.h \
    step/lvox3_stepcomputepad.h \
    step/lvox3_stepmakeshootingpaternformls.h \
    step/lvox3_stepgridnormalisation.h \
    tools/lvox3_errorcode.h \
    tools/lvox3_gridtype.h \
    view/loadfileconfiguration.h \
    view/computeprofilesconfiguration.h \
    view/loadfileconfigutil.h \
    view/mergegridsconfiguration.h \
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
    tools/lvox3_thetaphishootingpattern.h


SOURCES += \
    datastructures/lvox3_implpointcloud.cpp \
    datastructures/lvox3_implshootingpattern.cpp \
    qtwidgets/multiselectcombobox.cpp \
    step/lvox3_stepcomputenormalizedprofiles.cpp \
    tools/lvox3_logger.cpp \
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
    step/lvox3_stepmakeshootingpaternformls.cpp \
    step/lvox3_stepgridnormalisation.cpp \
    view/loadfileconfiguration.cpp \
    view/computeprofilesconfiguration.cpp \
    view/loadfileconfigutil.cpp \
    tools/lvox3_scannerutils.cpp \
    view/mergegridsconfiguration.cpp \
    tools/3dgrid/abstract/lvox3_abstractgrid3d.cpp \
    tools/3dgrid/lvox3_grid3d.cpp \
    tools/3dgrid/lvox3_grid3d_sparse.cpp \
    tools/drawmanager/lvox3_standardgrid3d_sparsedrawmanager.cpp \
    tools/drawmanager/lvox3_standardgrid3ddrawmanager.cpp \
    tools/exporter/lvox3_grid3dexporter.cpp \
    tools/reader/lvox3_reader_asciigrid3d.cpp \
    tools/lvox3_thetaphishootingpattern.cpp


TRANSLATIONS += languages/pluginlvoxv2_en.ts \
                languages/pluginlvoxv2_fr.ts

FORMS += \
    view/computegridsconfiguration.ui \
    view/computepadconfiguration.ui \
    view/computeprofilesconfiguration.ui \
    view/loadfileconfiguration.ui \
    view/mergegridsconfiguration.ui
RESOURCES += \
    resources.qrc

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../liblvox-qt/release/ -lliblvox-qt
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../liblvox-qt/debug/ -lliblvox-qt
#else:unix: LIBS += -L$$PWD/../liblvox-qt/ -lliblvox-qt
#
#INCLUDEPATH += $$PWD/../liblvox-qt/release/include
#DEPENDPATH += $$PWD/../liblvox-qt/release/
#
#LIBS += -L$$PWD/../liblvox-qt/release/

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../liblvox-qt/release/ -lliblvox-qt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../liblvox-qt/debug/ -lliblvox-qt
else:unix: LIBS += -L$$PWD/../liblvox-qt/ -lliblvox-qt

INCLUDEPATH += $$PWD/../liblvox-qt/release/include
INCLUDEPATH += $$PWD/../liblvox-qt
#DEPENDPATH += $$PWD/../liblvox-qt/release/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../liblvox-qt/release/libliblvox-qt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../liblvox-qt/debug/libliblvox-qt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../liblvox-qt/release/liblvox-qt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../liblvox-qt/debug/liblvox-qt.lib
else:unix: PRE_TARGETDEPS += $$PWD/../liblvox-qt/libliblvox-qt.a

DEPENDPATH += $${INCLUDEPATH}


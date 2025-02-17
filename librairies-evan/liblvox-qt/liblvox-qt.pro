CONFIG -= qt

TEMPLATE = lib

CONFIG += c++14
CONFIG += staticlib
# Expected path to install/<triplet> for VCPKG, only compatible with x64-windows.
# Won't compile using 32 bits
INCLUDEPATH += $$(VCPKG)\include\eigen3

# Release:DESTDIR = $$PWD/bin/Release

# Debug:DESTDIR = $$PWD/bin/Debug

# Release:message($$DESTDIR)
# cmath compatibility
DEFINES += _USE_MATH_DEFINES

# Qt handles poorly same-name files

SOURCES += \
    datastructures/ray.cpp \
    reducers/base.cpp \
    reducers/maxni.cpp \
    reducers/maxrdi.cpp \
    reducers/sumratio.cpp \
    reducers/maxtrust.cpp \
    reducers/weightedrdi.cpp \
    reducers/maxtrustratio.cpp \
    tools/logger.cpp \
    tools/pointclouditerator.cpp \
    traversal/propagation/grid3dpropagationalgorithm.cpp \
    traversal/propagation/visitor/trustinterpolationvisitor.cpp \
    traversal/propagation/visitor/distanceinterpolationvisitor.cpp \
    workers/interpolation/distance.cpp \
    workers/manager.cpp \
    workers/grids/hits.cpp \
    workers/grids/before.cpp \
    workers/grids/theoretical.cpp \
    workers/interpolation/zaverage.cpp \
    workers/interpolation/trustfactor.cpp \
    algorithms/computepad.cpp \
    algorithms/mergegrids.cpp \
    algorithms/computelvoxgrids.cpp \
    algorithms/interpolatedistances.cpp \
    workers/workerbase.cpp

HEADERS += \
    datastructures/grid3d.h \
    datastructures/pointcloudscene.h \
    datastructures/ray.h \
    datastructures/shootingpattern.h \
    datastructures/types.h \
    factory/factory.hpp \
    reducers/base.h \
    reducers/maxni.h \
    reducers/maxrdi.h \
    reducers/maxtrust.h \
    reducers/reducers.h \
    reducers/sumratio.h \
    reducers/weightedrdi.h \
    reducers/maxtrustratio.h \
    tools/logger.h \
    tools/pointclouditerator.h \
    tools/wootranversal.hpp \
    traversal/propagation/grid3dpropagationalgorithm.h \
    traversal/propagation/visitor/distanceinterpolationvisitor.h \
    traversal/propagation/visitor/propagationvisitor.h \
    traversal/propagation/visitor/propagationvisitorcontext.h \
    traversal/propagation/visitor/trustinterpolationvisitor.h \
    workers/interpolation/distance.h \
    workers/manager.h \
    workers/grids/hits.h \
    workers/grids/before.h \
    workers/grids/theoretical.h \
    workers/interpolation/zaverage.h \
    workers/interpolation/trustfactor.h \
    algorithms/mergegrids.h \
    algorithms/computepad.h \
    algorithms/computelvoxgrids.h \
    algorithms/interpolatedistances.h \
    workers/workerbase.h

RESOURCES += README.md

CONFIG(debug, debug|release){
    BUILDMODE = debug
} else {
    BUILDMODE = release
}
message($$PWD)
for(header, $$list($$HEADERS)) {
  path = $$PWD/$$BUILDMODE/include/liblvox/$$dirname(header)
  pathname = $$replace(path,/,)
  pathname = $$replace(pathname,\.,)
  pathname = $$replace(pathname,:,)
  file = headers_$${pathname}
  eval($${file}.files += $$header)
  eval($${file}.path = $$path)
  INSTALLS *= $${file}
}

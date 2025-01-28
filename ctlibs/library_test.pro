TEMPLATE      = subdirs
SUBDIRS       = ctlibmodels/models_test \
                ctlibstructure/structure_test \
				ctlibclouds/clouds_test \
				ctlibstep/step_test
CONFIG        += ordered

win32-msvc2013 {
    message( "msvc 2013 detected" )
    QMAKE_CXXFLAGS += /FS
}

TEMPLATE      = subdirs
SUBDIRS       = ctlibmodels \
                ctlibmodelsextraviews \
                ctliblog \
                ctlibwidget \
                ctlibclouds \
                ctlibstructure \
                ctlibstep \
                ctlibmath \
                ctlibstructureaddon \
                ctlibstepaddon \
                ctlibfilters \
                ctlibmetrics \
                ctlibstructurewidget \
                ctlibexporter \
                ctlibreader \
                ctlibaction \
                ctlibstdactions \
                ctlibplugin \
                ctlibio \
                ctliblas \
                ctliblaz \
                ctlibcore

ctlibmodelsextraviews.depends = ctlibmodels
ctlibstructure.depends = ctlibmodels
ctlibstep.depends = ctlibmodelsextraviews ctliblog ctlibwidget ctlibstructure
ctlibstructureaddon.depends = ctlibclouds ctliblog ctlibmath ctlibstructure
ctlibstepaddon.depends = ctlibstep ctlibstructureaddon
ctlibfilters.depends = ctliblog ctlibwidget ctlibstructureaddon
ctlibmetrics.depends = ctliblog ctlibwidget ctlibstructureaddon
ctlibstructurewidget.depends = ctlibstructure ctlibstep
ctlibexporter.depends = ctlibmodelsextraviews ctlibwidget ctlibstructureaddon
ctlibreader.depends = ctlibstructure ctlibstructureaddon ctlibwidget
ctlibstdactions.depends = ctliblog ctlibclouds ctlibmath ctlibstructure ctlibaction
ctlibplugin.depends = ctlibstep ctlibfilters ctlibmetrics ctlibexporter ctlibreader ctlibaction
ctliblas.depends = ctlibstructureaddon ctlibfilters ctlibmetrics ctlibexporter ctlibreader
ctliblaz.depends = ctlibstructureaddon ctlibreader ctlibexporter ctliblas
ctlibio.depends = ctlibplugin ctlibstructureaddon ctlibexporter ctlibreader ctliblas ctliblaz
ctlibcore.depends = ctlibplugin

# Optional use of Point Cloud Library (PCL)
MUST_USE_PCL = 1

!isEmpty(MUST_USE_PCL) {
    SUBDIRS += ctlibpcl
    ctlibpcl.depends = ctlibplugin ctliblog ctlibclouds ctlibstructureaddon ctlibreader
}

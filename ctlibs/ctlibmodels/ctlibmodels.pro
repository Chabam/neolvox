QT = core gui

contains ( QT_VERSION, "^5.*" ) {
    QT *= widgets
}

include(../../config/library_shared.pri)

TARGET = ctlibmodels

DEFINES += CTLIBMODELS_LIBRARY

HEADERS += ct_model/abstract/ct_abstractgroupmodelt.h \
    ct_model/abstract/ct_abstractgroupmodelt.hpp \
    ct_model/abstract/ct_abstractitemattributemodelt.h \
    ct_model/abstract/ct_abstractitemattributemodelt.hpp \
    ct_model/abstract/ct_abstractmodel.h \
    ct_model/abstract/ct_abstractsingularitemmodelt.h \
    ct_model/abstract/ct_abstractsingularitemmodelt.hpp \
    ct_model/inModel/ct_inresultmodelgroup.h \
    ct_model/inModel/ct_inresultmodelgrouptocopy.h \
    ct_model/inModel/ct_inresultmodelnotneedinputresult.h \
    ct_model/inModel/ct_instdgroupmodel.h \
    ct_model/inModel/ct_instditemattributemodel.h \
    ct_model/inModel/ct_instdsingularitemmodel.h \
    ct_model/inModel/ct_inzeroormoregroupmodel.h \
    ct_model/inModel/abstract/ct_inabstractgroupmodel.h \
    ct_model/inModel/abstract/ct_inabstractitemattributemodel.h \
    ct_model/inModel/abstract/ct_inabstractitemmodel.h \
    ct_model/inModel/abstract/ct_inabstractmodel.h \
    ct_model/inModel/abstract/ct_inabstractresultmodel.h \
    ct_model/inModel/abstract/ct_inabstractsingularitemmodel.h \
    ct_model/inModel/abstract/def_ct_abstractgroupmodelin.h \
    ct_model/inModel/abstract/def_ct_abstractitemdrawablemodelin.h \
    ct_model/inModel/abstract/def_ct_inabstractitemattributemodel.h \
    ct_model/inModel/tools/ct_instdmodelpossibility.h \
    ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h \
    ct_model/inModel/tools/ct_instdresultmodelpossibility.h \
    ct_model/outModel/abstract/ct_outabstractgroupmodel.h \
    ct_model/outModel/abstract/ct_outabstractitemattributemodel.h \
    ct_model/outModel/abstract/ct_outabstractitemmodel.h \
    ct_model/outModel/abstract/ct_outabstractmodel.h \
    ct_model/outModel/abstract/ct_outabstractresultmodel.h \
    ct_model/outModel/abstract/ct_outabstractresultmodelgroup.h \
    ct_model/outModel/abstract/ct_outabstractsingularitemmodel.h \
    ct_model/outModel/abstract/def_ct_abstractgroupmodelout.h \
    ct_model/outModel/abstract/def_ct_abstractitemdrawablemodelout.h \
    ct_model/outModel/abstract/def_ct_outabstractitemattributemodel.h \
    ct_model/outModel/ct_outopfnodegroupmodel.h \
    ct_model/outModel/ct_outresultmodelgroup.h \
    ct_model/outModel/ct_outresultmodelgroupcopy.h \
    ct_model/outModel/ct_outstdgroupmodel.h \
    ct_model/outModel/ct_outstditemattributemodel.h \
    ct_model/outModel/ct_outstdsingularitemmodel.h \
    #ct_model/tools/ct_modelsaverestorehelper.h \
    #ct_model/tools/ct_modelsearchhelper.h \
    #ct_model/tools/ct_modelselectionhelper.h \
    ctlibmodels_global.h \
    ct_model/interfacesformodels.h \
    ct_model/outModel/ct_outresultmodelgroup.hpp \
    ct_model/outModel/ct_outstdsingularitemmodel.hpp \
    ct_model/outModel/ct_outstdgroupmodel.hpp \
    ct_model/outModel/ct_outopfnodegroupmodel.hpp \
    ct_model/inModel/handle/ct_handleinitem.h \
    ct_model/outModel/handle/ct_handleoutitem.h \
    ct_model/handle/ct_handlewithmodelt.h \
    ct_model/inModel/handle/ct_handleinresult.h \
    ct_model/inModel/handle/ct_handleingroup.h \
    ct_model/inModel/manager/ct_inmodelstructuremanager.h \
    ct_model/inModel/handle/ct_handleinitemattribute.h \
    ct_model/inModel/handle/ct_handleinzeroormoregroup.h \
    ct_model/inModel/handle/ct_handleinresultcopy.h \
    ct_model/outModel/tools/ct_outresultmodelgrouptocopypossibilities.h \
    ct_model/outModel/manager/ct_outmodelstructuremanager.h \
    ct_model/outModel/handle/ct_handleoutresult.h \
    ct_model/outModel/ct_outstditemattributemodel.hpp \
    tools/sfinae.h \
    ct_model/outModel/handle/ct_handleoutgroup.h \
    ct_model/handle/ct_handlewithmultiplemodelt.h \
    ct_model/outModel/handle/ct_handleoutitemattribute.h \
    tools/modelsassert.h \
    ct_model/inModel/handle/ct_handleinbase.h \
    ct_model/inModel/tools/ct_inresultmodelconfigurationmanager.h \
    ct_model/inModel/views/ctg_inmodelpossibilitieschoice.h \
    ct_model/inModel/views/ctg_inresultmodelconfiguration.h \
    ct_model/inModel/views/ctg_inresultmodelpossibilities.h \
    ct_model/inModel/tools/ct_modelselectionhelper.h \
    ../settingsinterfaces.h \
    ct_model/handle/ct_handlewithmultipleabstractmodel.h \
    ct_model/inModel/handle/abstract/ct_abstractinhandlewithmodelt.h \
    ct_model/inModel/handle/ct_inhandleminmaxwithmodelt.h \
    ct_model/inModel/views/iinmodelpossibilitieschoice.h \
    ct_categories/abstract/ct_abstractcategory.h \
    ct_categories/tools/ct_categorymanager.h \
    ct_tools/ct_typeinfo.h

SOURCES += \ 
    ct_model/inModel/ct_inresultmodelgroup.cpp \
    ct_model/inModel/ct_inresultmodelgrouptocopy.cpp \
    ct_model/inModel/ct_inresultmodelnotneedinputresult.cpp \
    ct_model/inModel/ct_instdgroupmodel.cpp \
    ct_model/inModel/ct_instditemattributemodel.cpp \
    ct_model/inModel/ct_instdsingularitemmodel.cpp \
    ct_model/inModel/ct_inzeroormoregroupmodel.cpp \
    ct_model/inModel/abstract/ct_inabstractattributemodel.cpp \
    ct_model/inModel/abstract/ct_inabstractgroupmodel.cpp \
    ct_model/inModel/abstract/ct_inabstractitemmodel.cpp \
    ct_model/inModel/abstract/ct_inabstractresultmodel.cpp \
    ct_model/inModel/abstract/ct_inabstractsingularitemmodel.cpp \
    ct_model/inModel/tools/ct_instdmodelpossibility.cpp \
    ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.cpp \
    ct_model/inModel/tools/ct_instdresultmodelpossibility.cpp \
    ct_model/outModel/abstract/ct_outabstractgroupmodel.cpp \
    ct_model/outModel/abstract/ct_outabstractitemattributemodel.cpp \
    ct_model/outModel/abstract/ct_outabstractitemmodel.cpp \
    ct_model/outModel/abstract/ct_outabstractresultmodel.cpp \
    ct_model/outModel/abstract/ct_outabstractresultmodelgroup.cpp \
    ct_model/outModel/abstract/ct_outabstractsingularitemmodel.cpp \
    ct_model/abstract/ct_abstractmodel.cpp \
    ct_model/inModel/abstract/ct_inabstractmodel.cpp \
    ct_model/outModel/abstract/ct_outabstractmodel.cpp \
    ct_model/outModel/ct_outresultmodelgroupcopy.cpp \
    ct_model/inModel/manager/ct_inmodelstructuremanager.cpp \
    ct_model/outModel/tools/ct_outresultmodelgrouptocopypossibilities.cpp \
    ct_model/outModel/manager/ct_outmodelstructuremanager.cpp \
    ct_model/inModel/tools/ct_inresultmodelconfigurationmanager.cpp \
    ct_model/inModel/views/ctg_inmodelpossibilitieschoice.cpp \
    ct_model/inModel/views/ctg_inresultmodelconfiguration.cpp \
    ct_model/inModel/views/ctg_inresultmodelpossibilities.cpp \
    ct_model/inModel/tools/ct_modelselectionhelper.cpp \
    ct_categories/abstract/ct_abstractcategory.cpp \
    ct_categories/tools/ct_categorymanager.cpp

FORMS += \
    ct_model/inModel/views/ctg_inmodelpossibilitieschoice.ui \
    ct_model/inModel/views/ctg_inresultmodelconfiguration.ui \
    ct_model/inModel/views/ctg_inresultmodelpossibilities.ui

TRANSLATIONS += languages/ctlibmodels_fr.ts \
                languages/ctlibmodels_en.ts

RESOURCES += \
    icons.qrc

set(CTLIBMODELS_SOURCES
    ct_model/inModel/ct_inresultmodelgroup.cpp
    ct_model/inModel/ct_inresultmodelgrouptocopy.cpp
    ct_model/inModel/ct_inresultmodelnotneedinputresult.cpp
    ct_model/inModel/ct_instdgroupmodel.cpp
    ct_model/inModel/ct_instditemattributemodel.cpp
    ct_model/inModel/ct_instdsingularitemmodel.cpp
    ct_model/inModel/ct_inzeroormoregroupmodel.cpp
    ct_model/inModel/abstract/ct_inabstractattributemodel.cpp
    ct_model/inModel/abstract/ct_inabstractgroupmodel.cpp
    ct_model/inModel/abstract/ct_inabstractitemmodel.cpp
    ct_model/inModel/abstract/ct_inabstractresultmodel.cpp
    ct_model/inModel/abstract/ct_inabstractsingularitemmodel.cpp
    ct_model/inModel/tools/ct_instdmodelpossibility.cpp
    ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.cpp
    ct_model/inModel/tools/ct_instdresultmodelpossibility.cpp
    ct_model/outModel/abstract/ct_outabstractgroupmodel.cpp
    ct_model/outModel/abstract/ct_outabstractitemattributemodel.cpp
    ct_model/outModel/abstract/ct_outabstractitemmodel.cpp
    ct_model/outModel/abstract/ct_outabstractresultmodel.cpp
    ct_model/outModel/abstract/ct_outabstractresultmodelgroup.cpp
    ct_model/outModel/abstract/ct_outabstractsingularitemmodel.cpp
    ct_model/abstract/ct_abstractmodel.cpp
    ct_model/inModel/abstract/ct_inabstractmodel.cpp
    ct_model/outModel/abstract/ct_outabstractmodel.cpp
    ct_model/outModel/ct_outresultmodelgroupcopy.cpp
    ct_model/inModel/manager/ct_inmodelstructuremanager.cpp
    ct_model/outModel/tools/ct_outresultmodelgrouptocopypossibilities.cpp
    ct_model/outModel/manager/ct_outmodelstructuremanager.cpp
    ct_model/inModel/tools/ct_inresultmodelconfigurationmanager.cpp
    ct_model/inModel/views/ctg_inmodelpossibilitieschoice.cpp
    ct_model/inModel/views/ctg_inresultmodelconfiguration.cpp
    ct_model/inModel/views/ctg_inresultmodelpossibilities.cpp
    ct_model/inModel/tools/ct_modelselectionhelper.cpp
    ct_categories/abstract/ct_abstractcategory.cpp
    ct_categories/tools/ct_categorymanager.cpp
)

set(CTLIBMODELS_RESOURCES
    icons.qrc
)

set(CTLIBMODELS_FORMS
    ct_model/inModel/views/ctg_inmodelpossibilitieschoice.ui
    ct_model/inModel/views/ctg_inresultmodelconfiguration.ui
    ct_model/inModel/views/ctg_inresultmodelpossibilities.ui
)

set(CTLIBMODELS_TRANSLATIONS
    languages/ctlibmodels_fr.ts
    languages/ctlibmodels_en.ts
)

add_library(ctlibmodels SHARED
    ${CTLIBMODELS_SOURCES}
    ${CTLIBMODELS_RESOURCES}
    ${CTLIBMODELS_FORMS}
    ${CTLIBMODELS_TRANSLATIONS}
)

target_include_directories(ctlibmodels PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibmodels PRIVATE CTLIBMODELS_LIBRARY)
target_link_libraries(ctlibmodels
	ctlibcommon
	Qt5::Core
	Qt5::Widgets
)

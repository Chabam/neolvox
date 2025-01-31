project(ctlibstructure)

set(CTLIBSTRUCTURE_HEADERS
    ctlibstructure_global.h
    ct_global/ct_structurecontext.h
    ct_item/abstract/ct_abstractitem.h
    ct_itemattributes/abstract/ct_abstractitemattribute.h
    ct_itemattributes/abstract/ct_abstractitemattributet.h
    ct_itemattributes/abstract/ct_abstractitemattributet.hpp
    ct_itemdrawable/abstract/ct_abstractitemdrawable.h
    ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h
    ct_result/abstract/ct_abstractresult.h
    ct_itemattributes/tools/ct_defaultitemattributemanager.h
    ct_itemattributes/tools/ct_iacreator.h
    ct_itemattributes/tools/ct_itemattributecontainer.h
    ct_itemdrawable/tools/drawmanager/abstract/ct_abstractitemdrawabledrawmanager.h
    ct_itemdrawable/tools/drawmanager/ct_standardstandarditemgroupdrawmanager.h
    ct_itemdrawable/tools/ct_itemdrawableconfiguration.h
    ct_itemattributes/ct_stditemattributet.h
    ct_itemattributes/ct_stditemattributet.hpp
    ct_itemattributes/ct_stditemattributewrappert.h
    ct_itemattributes/ct_stditemattributewrappert.hpp
    ct_tools/ct_numerictostringconversiont.h
    ct_tools/ct_staticmethodinvoker.h
    ct_categories/ct_stdcategory.h
    ct_itemdrawable/abstract/ct_abstractgeometricalitem.h
    ct_result/ct_resultgroup.h
    ct_tools/ct_containert.h
    ct_tools/ct_containert.hpp
    ct_itemdrawable/ct_standarditemgroup.h
    ct_handle/ct_handleoutresultgroup.h
    ct_handle/ct_handleoutstdgroup.h
    ct_handle/ct_handleoutstditemattribute.h
    ct_handle/ct_handleoutstditemattributewrapper.h
    ct_handle/ct_handleinresultgroup.h
    ct_handle/ct_handleinstdgroup.h
    ct_handle/ct_handleinsingularitem.h
    ct_handle/ct_handleoutsingularitem.h
    ct_handle/ct_handleinstditemattribute.h
    ct_handle/ct_handleinstdzeroormoregroup.h
    ct_handle/ct_handleinstditemattributet.h
    ct_handle/ct_handleinresultgroupcopy.h
    ct_iterator/ct_multimodeliteratorstdstyle.h
    ct_iterator/ct_singlemodeliteratorstdstyleforresultgroup.h
    ct_tools/ct_handleiterationdecorator.h
    ct_tools/ct_handleiteratort.h
    ct_iterator/ct_multicollectioniteratorstdstyle.h
    ct_iterator/ct_singlecollectioniteratorstdstyle.h
)

set(CTLIBSTRUCTURE_SOURCES
    ct_global/ct_structurecontext.cpp
    ct_item/abstract/ct_abstractitem.cpp
    ct_itemattributes/abstract/ct_abstractitemattribute.cpp
    ct_itemdrawable/abstract/ct_abstractitemdrawable.cpp
    ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.cpp
    ct_result/abstract/ct_abstractresult.cpp
    ct_itemattributes/tools/ct_defaultitemattributemanager.cpp
    ct_itemattributes/tools/ct_itemattributecontainer.cpp
    ct_itemdrawable/tools/drawmanager/abstract/ct_abstractitemdrawabledrawmanager.cpp
    ct_itemdrawable/tools/drawmanager/ct_standardstandarditemgroupdrawmanager.cpp
    ct_itemdrawable/tools/ct_itemdrawableconfiguration.cpp
    ct_categories/ct_stdcategory.cpp
    ct_itemdrawable/abstract/ct_abstractgeometricalitem.cpp
    ct_result/ct_resultgroup.cpp
    ct_itemdrawable/ct_standarditemgroup.cpp
)

set(CTLIBSTRUCTURE_TRANSLATIONS
    languages/ctlibstructure_fr.ts
    languages/ctlibstructure_en.ts
)

add_library(ctlibstructure SHARED
    ${CTLIBSTRUCTURE_HEADERS}
    ${CTLIBSTRUCTURE_SOURCES}
    ${CTLIBSTRUCTURE_TRANSLATIONS}
)

target_include_directories(ctlibstructure PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_definitions(ctlibstructure PRIVATE CTLIBSTRUCTURE_LIBRARY)
target_link_libraries(ctlibstructure
    ctlibcommon
    ctlibmodels
    Eigen3::Eigen
    Qt5::Core
    Qt5::Widgets
)

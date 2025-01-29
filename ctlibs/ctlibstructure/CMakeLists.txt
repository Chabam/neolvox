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

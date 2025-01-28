#include "ct_structurecontext.h"

#include "ct_itemattributes/tools/ct_defaultitemattributemanager.h"
#include "ct_categories/tools/ct_categorymanager.h"
#include "ct_categories/ct_stdcategory.h"

CT_StructureContext* CT_StructureContext::UNIQUE_INSTANCE = nullptr;

CT_StructureContext::CT_StructureContext()
{
    m_categoryManager = nullptr;
    m_defaultItemAttributeManager = nullptr;
}

CT_StructureContext::~CT_StructureContext()
{
    delete m_defaultItemAttributeManager;
    delete m_categoryManager;
}

void CT_StructureContext::init()
{
    m_categoryManager = new CT_CategoryManager();

    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataId(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataDisplayableName(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataFileName(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataCoordinate(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataX(), CT_AbstractCategory::staticInitDataCoordinate()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataY(), CT_AbstractCategory::staticInitDataCoordinate()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataZ(), CT_AbstractCategory::staticInitDataCoordinate()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataCx(), CT_AbstractCategory::staticInitDataX()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataCy(), CT_AbstractCategory::staticInitDataY()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataCz(), CT_AbstractCategory::staticInitDataZ()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataDirection(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataXDirection(), QStringList{CT_AbstractCategory::staticInitDataX(), CT_AbstractCategory::staticInitDataDirection()}));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataYDirection(), QStringList{CT_AbstractCategory::staticInitDataY(), CT_AbstractCategory::staticInitDataDirection()}));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataZDirection(), QStringList{CT_AbstractCategory::staticInitDataZ(), CT_AbstractCategory::staticInitDataDirection()}));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataDimension(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataXDimension(), QStringList{CT_AbstractCategory::staticInitDataX(), CT_AbstractCategory::staticInitDataDimension()}));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataYDimension(), QStringList{CT_AbstractCategory::staticInitDataY(), CT_AbstractCategory::staticInitDataDimension()}));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataZDimension(), QStringList{CT_AbstractCategory::staticInitDataZ(), CT_AbstractCategory::staticInitDataDimension()}));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataRadius(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataR2(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataSize(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataNumber(), CT_AbstractCategory::staticInitDataSize()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataLength(), CT_AbstractCategory::staticInitDataSize()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataWidth(), CT_AbstractCategory::staticInitDataLength()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataHeight(), CT_AbstractCategory::staticInitDataLength()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataArea(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataAngle(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataRatio(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataResolution(), CT_AbstractCategory::staticInitDataValue()));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataXResolution(), QStringList{CT_AbstractCategory::staticInitDataX(), CT_AbstractCategory::staticInitDataResolution()}));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataYResolution(), QStringList{CT_AbstractCategory::staticInitDataY(), CT_AbstractCategory::staticInitDataResolution()}));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataZResolution(), QStringList{CT_AbstractCategory::staticInitDataZ(), CT_AbstractCategory::staticInitDataResolution()}));
    m_categoryManager->registerCategory(new CT_StdCategory(CT_AbstractCategory::staticInitDataNa(), CT_AbstractCategory::staticInitDataValue()));

    m_defaultItemAttributeManager = new CT_DefaultItemAttributeManager();
}

void CT_StructureContext::informThatWeStartToDeleteMultipleCloud()
{
    // TODO : MK 22.11.18
    //repository()->begingDeleteMultiCloud();
}

void CT_StructureContext::informThatWeHaveFinishedToDeleteMultipleCloud()
{
    // TODO : MK 22.11.18
    //repository()->endDeleteMultiCloud();
}

CT_CategoryManager* CT_StructureContext::categoryManager() const
{
    return m_categoryManager;
}

CT_DefaultItemAttributeManager* CT_StructureContext::defaultItemAttributesManager() const
{
    return m_defaultItemAttributeManager;
}

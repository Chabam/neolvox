#include "ct_standardpointclusterdrawmanager.h"
#include "ct_itemdrawable/ct_pointcluster.h"

#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"
#include "ct_accessor/ct_pointaccessor.h"
#include "painterinterface.h"

const QString CT_StandardPointClusterDrawManager::INDEX_CONFIG_BARYCENTER_VISIBLE = CT_StandardPointClusterDrawManager::staticInitConfigBarycenterVisible();
const QString CT_StandardPointClusterDrawManager::INDEX_CONFIG_LINES_VISIBLE = CT_StandardPointClusterDrawManager::staticInitConfigLinesVisible();

CT_StandardPointClusterDrawManager::CT_StandardPointClusterDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_PointCluster::staticName() : drawConfigurationName)
{
}

void CT_StandardPointClusterDrawManager::draw(GraphicsViewInterface& view, PainterInterface& painter, const CT_AbstractItemDrawable& itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_PointCluster& item = static_cast<const CT_PointCluster&>(itemDrawable);

    if(drawConfiguration()->variableValue(INDEX_CONFIG_BARYCENTER_VISIBLE).toBool())
    {
        const CT_PointClusterBarycenter &barycenter = item.getBarycenter();
        painter.drawPoint(barycenter.x(), barycenter.y(), barycenter.z());
    }

    if(drawConfiguration()->variableValue(INDEX_CONFIG_LINES_VISIBLE).toBool())
    {
        CT_PointAccessor pointAccessor1;
        CT_PointAccessor pointAccessor2;
        const CT_AbstractPointCloudIndex* pointCloudIndex = item.pointCloudIndex();

        if(pointCloudIndex->size() > 1)
        {
            const size_t size = pointCloudIndex->size() - 1;

            for(size_t i=0 ; i<size; ++i)
            {
                const size_t index1 = size_t(pointCloudIndex->constIndexAt(i));
                const size_t index2 = size_t(pointCloudIndex->constIndexAt(i+1));

                const CT_Point& point1 = pointAccessor1.constPointAt(index1);
                const CT_Point& point2 = pointAccessor2.constPointAt(index2);

                painter.drawLine(point1(0), point1(1), point1(2), point2(0), point2(1), point2(2));
            }
        }
    }
}

CT_ItemDrawableConfiguration CT_StandardPointClusterDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardPointClusterDrawManager::staticInitConfigBarycenterVisible(), QObject::tr("Barycentre"), CT_ItemDrawableConfiguration::Bool, false );
    item.addNewConfiguration(CT_StandardPointClusterDrawManager::staticInitConfigLinesVisible(),QObject::tr("Polyligne"), CT_ItemDrawableConfiguration::Bool, false );

    return item;
}

QString CT_StandardPointClusterDrawManager::staticInitConfigBarycenterVisible()
{
    return "PTCL_BA";
}

QString CT_StandardPointClusterDrawManager::staticInitConfigLinesVisible()
{
    return "PTCL_LI";
}

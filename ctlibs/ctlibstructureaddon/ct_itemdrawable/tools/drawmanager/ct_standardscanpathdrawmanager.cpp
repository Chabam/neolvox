#include "ct_standardscanpathdrawmanager.h"
#include "ct_itemdrawable/ct_scanpath.h"
#include "painterinterface.h"

const QString CT_StandardScanPathDrawManager::INDEX_CONFIG_DRAW_POINTS = CT_StandardScanPathDrawManager::staticInitConfigDrawPoints();
const QString CT_StandardScanPathDrawManager::INDEX_CONFIG_DRAW_LINES = CT_StandardScanPathDrawManager::staticInitConfigDrawLines();
const QString CT_StandardScanPathDrawManager::INDEX_CONFIG_POINT_SIZE = CT_StandardScanPathDrawManager::staticInitConfigPointSize();

CT_StandardScanPathDrawManager::CT_StandardScanPathDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_ScanPath::staticName() : drawConfigurationName)
{
}

void CT_StandardScanPathDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_ScanPath &scanPath = dynamic_cast<const CT_ScanPath&>(itemDrawable);
    //double sizeCube = (double) (drawConfiguration()->variableValue(INDEX_CONFIG_POINT_SIZE).toInt()) / 100.0;
    const bool drawPoints = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_POINTS).toBool();
    const bool drawLines = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_LINES).toBool();

    const QList<CT_ScanPath::PathPoint>& path = scanPath.getPath();

    Eigen::Vector3d lastPoint;
    Eigen::Vector3d point;
    bool first = true;

    for (int i = 0 ; i < path.size() ; i++)
    {
        const CT_ScanPath::PathPoint& pp = path.at(i);
        lastPoint = point;
        point = pp._position;

        if(drawPoints)
        {
            painter.setPointSize(drawConfiguration()->variableValue(INDEX_CONFIG_POINT_SIZE).toInt());
            painter.drawPoint(point(0), point(1), point(2));
        }

        if(drawLines && !first)
        {
            painter.drawLine(lastPoint(0), lastPoint(1), lastPoint(2), point(0), point(1), point(2));
        }
        if (first) {first = false;}
    }
    painter.restoreDefaultPointSize();
}

CT_ItemDrawableConfiguration CT_StandardScanPathDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    //item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardScanPathDrawManager::staticInitConfigDrawPoints(), QObject::tr("Draw points"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(CT_StandardScanPathDrawManager::staticInitConfigDrawLines(), QObject::tr("Draw lines"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(CT_StandardScanPathDrawManager::staticInitConfigPointSize(), QObject::tr("Points size"), CT_ItemDrawableConfiguration::Int, 5);

    return item;
}

// PROTECTED //

QString CT_StandardScanPathDrawManager::staticInitConfigDrawPoints()
{
    return "SP_DP";
}

QString CT_StandardScanPathDrawManager::staticInitConfigDrawLines()
{
    return "SP_DL";
}

QString CT_StandardScanPathDrawManager::staticInitConfigPointSize()
{
    return "SP_S";
}

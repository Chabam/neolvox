#include "ct_standardreferencepointdrawmanager.h"

#include "ct_itemdrawable/ct_referencepoint.h"
#include "painterinterface.h"

const QString CT_StandardReferencePointDrawManager::INDEX_CONFIG_BUFFER_VISIBLE = CT_StandardReferencePointDrawManager::staticInitConfigBufferVisible();
const QString CT_StandardReferencePointDrawManager::INDEX_CONFIG_FACTOR = CT_StandardReferencePointDrawManager::staticInitConfigFactor();
const QString CT_StandardReferencePointDrawManager::INDEX_CONFIG_POINT_SIZE = CT_StandardReferencePointDrawManager::staticInitConfigPointSize();

CT_StandardReferencePointDrawManager::CT_StandardReferencePointDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_ReferencePoint::staticName() : drawConfigurationName)
{
}

void CT_StandardReferencePointDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_ReferencePoint &item = static_cast<const CT_ReferencePoint&>(itemDrawable);

    const double sizeCube = (double) (drawConfiguration()->variableValue(INDEX_CONFIG_POINT_SIZE).toInt()) / 100.0;

    painter.setPointSize(drawConfiguration()->variableValue(INDEX_CONFIG_POINT_SIZE).toInt());
    //painter.drawPoint(item.centerX(), item.centerY(), item.centerZ());
    painter.drawCube(item.centerX() - sizeCube, item.centerY() - sizeCube, item.centerZ() - sizeCube,item.centerX() + sizeCube, item.centerY() + sizeCube, item.centerZ() + sizeCube, GL_FRONT_AND_BACK, GL_FILL);

    if(drawConfiguration()->variableValue(INDEX_CONFIG_BUFFER_VISIBLE).toBool())
        painter.drawCircle(item.x(), item.y(), item.z(), item.xyBuffer()*drawConfiguration()->variableValue(INDEX_CONFIG_FACTOR).toDouble());

    painter.restoreDefaultPointSize();
}

CT_ItemDrawableConfiguration CT_StandardReferencePointDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    //item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardReferencePointDrawManager::staticInitConfigBufferVisible(), QObject::tr("Buffer"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(CT_StandardReferencePointDrawManager::staticInitConfigFactor(), QObject::tr("Facteur multiplicatif"), CT_ItemDrawableConfiguration::Double, 1.0);
    item.addNewConfiguration(CT_StandardReferencePointDrawManager::staticInitConfigPointSize(), QObject::tr("Taille de point"), CT_ItemDrawableConfiguration::Int, 10);

    return item;
}

// PROTECTED //

QString CT_StandardReferencePointDrawManager::staticInitConfigBufferVisible()
{
    return "RP_BV";
}

QString CT_StandardReferencePointDrawManager::staticInitConfigFactor()
{
    return "RP_F";
}

QString CT_StandardReferencePointDrawManager::staticInitConfigPointSize()
{
    return "RP_S";
}



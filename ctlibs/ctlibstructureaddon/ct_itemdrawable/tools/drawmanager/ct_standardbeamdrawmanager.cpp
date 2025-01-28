#include "ct_standardbeamdrawmanager.h"

#include "ct_itemdrawable/ct_beam.h"
#include "painterinterface.h"

const QString CT_StandardBeamDrawManager::INDEX_CONFIG_LENGTH_OF_BEAM = CT_StandardBeamDrawManager::staticInitConfigLengthOfRay();

CT_StandardBeamDrawManager::CT_StandardBeamDrawManager(const QString& drawConfigurationName) :
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager(drawConfigurationName.isEmpty() ? CT_Beam::staticName() : drawConfigurationName)
{   
}

void CT_StandardBeamDrawManager::draw(GraphicsViewInterface &view,
                                      PainterInterface &painter,
                                      const CT_AbstractItemDrawable &itemDrawable) const
{
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::draw(view, painter, itemDrawable);

    const CT_Beam& item = static_cast<const CT_Beam&>(itemDrawable);

    double tMax = drawConfiguration()->variableValue(INDEX_CONFIG_LENGTH_OF_BEAM).toDouble();
    painter.drawLine( item.origin().x(),
                      item.origin().y(),
                      item.origin().z(),
                      item.origin().x()+tMax*item.direction().x(),
                      item.origin().y()+tMax*item.direction().y(),
                      item.origin().z()+tMax*item.direction().z());
}

CT_ItemDrawableConfiguration CT_StandardBeamDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardBeamDrawManager::staticInitConfigLengthOfRay(), QObject::tr("Length of the ray"), CT_ItemDrawableConfiguration::Double, 1.0);

    return item;
}

// PROTECTED //

QString CT_StandardBeamDrawManager::staticInitConfigLengthOfRay()
{
    return "BEAM_LOR";
}


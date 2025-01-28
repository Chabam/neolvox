#include "ct_standardcylinderdrawmanager.h"

#include "ct_itemdrawable/ct_cylinder.h"
#include "painterinterface.h"

const QString CT_StandardCylinderDrawManager::INDEX_CONFIG_DRAW_CYLINDER = CT_StandardCylinderDrawManager::staticInitConfigDrawCylinder();
const QString CT_StandardCylinderDrawManager::INDEX_CONFIG_DRAW_AXE = CT_StandardCylinderDrawManager::staticInitConfigDrawAxe();
const QString CT_StandardCylinderDrawManager::INDEX_CONFIG_DRAW_MEDIAN_CIRCLE = CT_StandardCylinderDrawManager::staticInitConfigDrawMedianCircle();
const QString CT_StandardCylinderDrawManager::INDEX_CONFIG_TRANSPARENCY_LEVEL = CT_StandardCylinderDrawManager::staticInitConfigTransparencyLevel();

CT_StandardCylinderDrawManager::CT_StandardCylinderDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Cylinder::staticName() : drawConfigurationName)
{
}

void CT_StandardCylinderDrawManager::draw(GraphicsViewInterface& view, PainterInterface& painter, const CT_AbstractItemDrawable& itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Cylinder& item = static_cast<const CT_Cylinder&>(itemDrawable);

    const Eigen::Vector3d& center = item.getCenter();
    const Eigen::Vector3d& direction = item.getDirection();

    if(drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_CYLINDER).toBool())
    {
        const QColor color = painter.getColor();

        int transparence = drawConfiguration()->variableValue(INDEX_CONFIG_TRANSPARENCY_LEVEL).toInt();
        if (transparence < 0) { transparence = 0; }
        if (transparence > 255) { transparence = 255; }

        //Eigen::Vector3d bottom = center - (direction * item.getHeight()/2.0);

        painter.setColor(QColor(color.red(), color.green(), color.blue(), transparence));
        painter.drawCylinder3D(center, direction, item.getRadius(), item.getHeight());

        painter.setColor(color);
    }

    if(drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_AXE).toBool())
        painter.drawLine(center.x(), center.y(), center.z(), direction.x()+center.x(), direction.y()+center.y(), direction.z()+center.z());

    if(drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_MEDIAN_CIRCLE).toBool())
        painter.drawCircle3D(center, direction, item.getRadius());
}

CT_ItemDrawableConfiguration CT_StandardCylinderDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardCylinderDrawManager::staticInitConfigDrawCylinder() ,QObject::tr("Dessiner le cylindre"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(CT_StandardCylinderDrawManager::staticInitConfigDrawAxe() ,QObject::tr("Dessiner l'axe"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(CT_StandardCylinderDrawManager::staticInitConfigDrawMedianCircle() , QObject::tr("Dessiner le cercle médian"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(CT_StandardCylinderDrawManager::staticInitConfigTransparencyLevel() , QObject::tr("Niveau de transparence [0;255]"), CT_ItemDrawableConfiguration::Double, 100);

    return item;
}

// PROTECTED //

QString CT_StandardCylinderDrawManager::staticInitConfigDrawCylinder()
{
    return "CYL_DC";
}

QString CT_StandardCylinderDrawManager::staticInitConfigDrawAxe()
{
    return "CYL_DA";
}

QString CT_StandardCylinderDrawManager::staticInitConfigDrawMedianCircle()
{
    return "CYL_DMC";
}

QString CT_StandardCylinderDrawManager::staticInitConfigTransparencyLevel()
{
    return "CYL_TL";
}

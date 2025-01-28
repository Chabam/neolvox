#include "ct_standardcolorcompositedrawmanager.h"

#include "ct_itemdrawable/ct_colorcomposite.h"
#include "ct_tools/ct_typeinfo.h"

const QString CT_StandardColorCompositeDrawManager::INDEX_CONFIG_MIN_RED= CT_StandardColorCompositeDrawManager::staticInitConfigMinRed();
const QString CT_StandardColorCompositeDrawManager::INDEX_CONFIG_MAX_RED = CT_StandardColorCompositeDrawManager::staticInitConfigMaxRed();
const QString CT_StandardColorCompositeDrawManager::INDEX_CONFIG_3D_MODE_ENABLED = CT_StandardColorCompositeDrawManager::staticInitConfig3DModeEnabled();
const QString CT_StandardColorCompositeDrawManager::INDEX_CONFIG_MAP_MODE_ENABLED = CT_StandardColorCompositeDrawManager::staticInitConfigMapModeEnabled();
const QString CT_StandardColorCompositeDrawManager::INDEX_CONFIG_MAP_MODE_ZLEVEL_ENABLED = CT_StandardColorCompositeDrawManager::staticInitConfigMapModeZLevelEnabled();
const QString CT_StandardColorCompositeDrawManager::INDEX_CONFIG_MAP_MODE_ZLEVEL_VALUE = CT_StandardColorCompositeDrawManager::staticInitConfigMapModeZLevelValue();

CT_StandardColorCompositeDrawManager::CT_StandardColorCompositeDrawManager(QString drawConfigurationName, bool mapMode, bool scale)
    : SuperClass(drawConfigurationName.isEmpty() ? CT_ColorComposite::staticName() : drawConfigurationName)
{
    _defaultMapMode = mapMode;
    _defaultScaleState = scale;
}

void CT_StandardColorCompositeDrawManager::draw(GraphicsViewInterface& view,
                                                PainterInterface& painter,
                                                const CT_AbstractItemDrawable& itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_ColorComposite& item = static_cast<const CT_ColorComposite&>(itemDrawable);

    const CT_Image2D<quint8>* red = item.getRedBand();
    const CT_Image2D<quint8>* green = item.getGreenBand();
    const CT_Image2D<quint8>* blue = item.getBlueBand();
    const CT_Image2D<float>*  zvalue = item.getZValueRaster();

    if (red != nullptr && green != nullptr && blue != nullptr)
    {
        const bool mode3D = drawConfiguration()->variableValue(INDEX_CONFIG_3D_MODE_ENABLED).toBool();
        const bool modeMap = drawConfiguration()->variableValue(INDEX_CONFIG_MAP_MODE_ENABLED).toBool();
        const bool fixerZ = drawConfiguration()->variableValue(INDEX_CONFIG_MAP_MODE_ZLEVEL_ENABLED).toBool();
        const double z = drawConfiguration()->variableValue(INDEX_CONFIG_MAP_MODE_ZLEVEL_VALUE).toDouble();
        const float minRed = drawConfiguration()->variableValue(INDEX_CONFIG_MIN_RED).toInt();
        const float maxRed = drawConfiguration()->variableValue(INDEX_CONFIG_MAX_RED).toInt();

        float ampliRed = maxRed - minRed;
        if (ampliRed < 0) {ampliRed = -ampliRed;}

        const double demiRes = red->resolution()/2.0;

        if (mode3D && zvalue != nullptr)
        {
            // sinon dessiner seulement les points
            double x, y;
            float value;

            for (int ly = 0 ; ly < red->ydim()-1 ; ly++)
            {
                for (int cx = 0 ; cx < red->xdim()-1 ; cx++)
                {
                    x = red->getCellCenterColCoord(cx);
                    y = red->getCellCenterLinCoord(ly);

                    value = zvalue->valueAtCoords(x, y);

                    if (!qFuzzyCompare(value, zvalue->NA()))
                    {
                        int r = int(red->value(cx, ly));
                        int g = int(green->value(cx, ly));
                        int b = int(blue->value(cx, ly));

                        if (r < minRed) {r = 0;}
                        else if (r > maxRed) {r = 255;}
                        else {r = int(255*(r - minRed)/ampliRed);}

                        if (g < minRed) {g = 0;}
                        else if (g > maxRed) {g = 255;}
                        else {g = int(255*(g - minRed)/ampliRed);}

                        if (b < minRed) {b = 0;}
                        else if (b > maxRed) {b = 255;}
                        else {b = int(255*(b - minRed)/ampliRed);}

                        painter.setColor(r, g, b);

                        painter.drawPoint(x, y, double(value));
                    }
                }
            }
        }

        if (modeMap) {
            double z_val = red->level();
            if (fixerZ) {z_val = z;}

            for (int cx = 0 ; cx < red->xdim() ; cx++)
            {
                for (int ly = 0 ; ly < red->ydim() ; ly++)
                {
                    double x = red->getCellCenterColCoord(cx);
                    double y = red->getCellCenterLinCoord(ly);

                    int r = red->value(cx, ly);
                    int g = green->value(cx, ly);
                    int b = blue->value(cx, ly);

                    if (r < minRed) {r = 0;}
                    else if (r > maxRed) {r = 255;}
                    else {r = int(255*(r - minRed)/ampliRed);}

                    if (g < minRed) {g = 0;}
                    else if (g > maxRed) {g = 255;}
                    else {g = int(255*(g - minRed)/ampliRed);}

                    if (b < minRed) {b = 0;}
                    else if (b > maxRed) {b = 255;}
                    else {b = int(255*(b - minRed)/ampliRed);}

                    painter.setColor(QColor(r, g, b));

                    Eigen::Vector2d tLeft(x - demiRes, y + demiRes);
                    Eigen::Vector2d bRight(tLeft(0) + red->resolution(), tLeft(1) - red->resolution());

                    painter.fillRectXY(tLeft, bRight, z_val);

                }
            }
        }
    }
}

CT_ItemDrawableConfiguration CT_StandardColorCompositeDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));

    // Adding lines to this config dialog box
    item.addNewConfiguration(staticInitConfigMinRed(), QObject::tr("Min Red"), CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigMaxRed(), QObject::tr("Max Red"), CT_ItemDrawableConfiguration::Int, 255);
    item.addNewConfiguration(staticInitConfigMapModeEnabled(), QObject::tr("Mode Raster"), CT_ItemDrawableConfiguration::Bool, _defaultMapMode);
    item.addNewConfiguration(staticInitConfigMapModeZLevelEnabled(), QObject::tr("Mode Raster : Fixer le niveau Z"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(staticInitConfigMapModeZLevelValue(), QObject::tr("Mode Raster : Niveau Z (m)"), CT_ItemDrawableConfiguration::Double, 0);

    item.addNewConfiguration(staticInitConfig3DModeEnabled(), QObject::tr("Mode 3D"), CT_ItemDrawableConfiguration::Bool, !_defaultMapMode);

    return item;
}

// PROTECTED //


QString CT_StandardColorCompositeDrawManager::staticInitConfigMinRed()
{
    return "CCOMP_MINR";
}

QString CT_StandardColorCompositeDrawManager::staticInitConfigMaxRed()
{
    return "CCOMP_MAXRE";
}


QString CT_StandardColorCompositeDrawManager::staticInitConfig3DModeEnabled()
{
    return "CCOMP_3DME";
}


QString CT_StandardColorCompositeDrawManager::staticInitConfigMapModeEnabled()
{
    return "CCOMP_MME";
}

QString CT_StandardColorCompositeDrawManager::staticInitConfigMapModeZLevelEnabled()
{
    return "CCOMP_MMZLE";
}

QString CT_StandardColorCompositeDrawManager::staticInitConfigMapModeZLevelValue()
{
    return "CCOMP_MMZLV";
}

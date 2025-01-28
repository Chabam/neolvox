#include "ct_standardgrid4ddrawmanager.h"

#include "ct_itemdrawable/ct_grid4d.h"
#include "painterinterface.h"

#include <QDebug>

// Initialise static attributes
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_W_DISPLAY_VALUE = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigWDisplayValue();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_WIRE_MODE_ENABLED = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigWireModeEnabled();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_LOW_THRESHOLDS_ENABLED = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigLowThresholdsEnabled();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_HIGH_THRESHOLDS_ENABLED = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigHighThresholdsEnabled();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_LOW_THRESHOLDS_VALUE = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigLowThresholdValue();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_HIGH_THRESHOLDS_VALUE = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigHighThresholdValue();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_REDUCTION_COEF = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigReductionCoef();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_TRANSPARENCY_ENABLED = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigEnableTransparency();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_TRANSPARENCY_VALUE = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigTransparencyValue();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_XINF = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigXinf();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_XSUP = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigXsup();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_YINF = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigYinf();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_YSUP = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigYsup();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_ZINF = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigZinf();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_ZSUP = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigZsup();
template< typename DataT > const QString CT_StandardGrid4DDrawManager<DataT>::INDEX_CONFIG_SHOW_TRUES_ONLY = CT_StandardGrid4DDrawManager<DataT>::staticInitConfigShowTrueOnly();

template< typename DataT >
CT_StandardGrid4DDrawManager<DataT>::CT_StandardGrid4DDrawManager(QString drawConfigurationName)
    : SuperClass( drawConfigurationName.isEmpty() ? CT_Grid4D<DataT>::staticName() : drawConfigurationName )
{

}

template< typename DataT >
void CT_StandardGrid4DDrawManager<DataT>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Grid4D<DataT> &item = dynamic_cast<const CT_Grid4D<DataT>&>(itemDrawable);

    // Getting the configuration values
    GLenum  drawingMode;
    bool    wireMode = this->drawConfiguration()->variableValue(INDEX_CONFIG_WIRE_MODE_ENABLED).toBool();
    bool    uselowThresh = this->drawConfiguration()->variableValue(INDEX_CONFIG_LOW_THRESHOLDS_ENABLED).toBool();
    bool    usehighThresh = this->drawConfiguration()->variableValue(INDEX_CONFIG_HIGH_THRESHOLDS_ENABLED).toBool();
    double  lowThresh = this->drawConfiguration()->variableValue(INDEX_CONFIG_LOW_THRESHOLDS_VALUE).toDouble();
    double  highThresh = this->drawConfiguration()->variableValue(INDEX_CONFIG_HIGH_THRESHOLDS_VALUE).toDouble();
    double  reductionCoef = this->drawConfiguration()->variableValue(INDEX_CONFIG_REDUCTION_COEF).toDouble();
    bool    useTransparency = this->drawConfiguration()->variableValue(INDEX_CONFIG_TRANSPARENCY_ENABLED).toBool();
    int     transparencyValue = this->drawConfiguration()->variableValue(INDEX_CONFIG_TRANSPARENCY_VALUE).toInt();

    int     ww  = this->drawConfiguration()->variableValue(INDEX_CONFIG_W_DISPLAY_VALUE).toInt();
    int     nXinf = this->drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_XINF).toInt();
    int     nXsup = this->drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_XSUP).toInt();
    int     nYinf = this->drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_YINF).toInt();
    int     nYsup = this->drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_YSUP).toInt();
    int     nZinf = this->drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_ZINF).toInt();
    int     nZsup = this->drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_ZSUP).toInt();

    if (nXsup > item.xdim()) {nXsup = item.xdim();}
    if (nYsup > item.ydim()) {nXsup = item.ydim();}
    if (nZsup > item.zdim()) {nXsup = item.zdim();}

    if (!useTransparency || (transparencyValue > 255)) {transparencyValue = 255;}
    if (transparencyValue < 0) {transparencyValue = 0;}

    if ( wireMode ) {drawingMode = GL_LINE;}
    else            {drawingMode = GL_FILL;}

    if (!uselowThresh) {lowThresh =  double(item.dataMin());}
    if (!usehighThresh) {highThresh =  double(item.dataMax());}

    bool drawAsMap = true;
    if (qFuzzyCompare(lowThresh, highThresh)) {drawAsMap = false;}

    QColor color = painter.getColor();

    // For each voxel of the grid
    double scaling = 240.0 / (highThresh - lowThresh);
    double offset = - (240*lowThresh)/(highThresh - lowThresh);

    int xdim = item.xdim();
    int ydim = item.ydim();
    int zdim = item.zdim();
    double demiResx = reductionCoef*item.xres() / 2.0;
    double demiResy = reductionCoef*item.yres() / 2.0;
    double demiResz = reductionCoef*item.zres() / 2.0;

    double xmin, ymin, zmin, xmax, ymax, zmax;

    // If w val > wwMax, do not display
    if ( ww <= item.wdim() )
    {
        // For each voxel of the grid
        for (int xx = nXinf ; xx < (xdim - nXsup) ; xx++)
        {
            for (int yy = nYinf ; yy < (ydim - nYsup)  ; yy++)
            {
                for (int zz = nZinf ; zz < (zdim - nZsup); zz++)
                {
                    size_t index;
                    if (item.index(ww, xx, yy, zz, index))
                    {
                        DataT data = item.valueAtIndex(index);

                        // Draw a cube if the value it contains is between the two thresholds
                        if ( data >= lowThresh && data <= highThresh )
                        {

                            if (drawAsMap && !itemDrawable.isSelected())
                            {
                                double h = double(qRound((data*scaling) + offset));
                                painter.setColor( QColor::fromHsv(int(h),255,255,transparencyValue) );
                            } else {
                                painter.setColor(QColor(255,255,255));
                            }

                            xmin = item.getCellCenterX(xx) - demiResx;
                            ymin = item.getCellCenterY(yy) - demiResy;
                            zmin = item.getCellCenterZ(zz) - demiResz;
                            xmax = item.getCellCenterX(xx) + demiResx;
                            ymax = item.getCellCenterY(yy) + demiResy;
                            zmax = item.getCellCenterZ(zz) + demiResz;

                            painter.drawCube( xmin, ymin, zmin, xmax, ymax, zmax, GL_FRONT_AND_BACK, drawingMode );
                        }
                    }

                    else
                    {
                        qDebug() << "Probleme d'index (drawmanager)";
                    }
                }
            }
        }
    }

    painter.setColor(color);
}

template< typename DataT >
CT_ItemDrawableConfiguration CT_StandardGrid4DDrawManager<DataT>::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addNewConfiguration(staticInitConfigWDisplayValue(), QObject::tr("W fixe"), CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigWireModeEnabled(), QObject::tr("Mode filaire"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(staticInitConfigLowThresholdsEnabled(), QObject::tr("Forcer limite basse"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(staticInitConfigLowThresholdValue(), QObject::tr("Limite basse (forcée)"), CT_ItemDrawableConfiguration::Double, 1 );
    item.addNewConfiguration(staticInitConfigHighThresholdsEnabled(), QObject::tr("Forcer limite haute"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(staticInitConfigHighThresholdValue(), QObject::tr("Limite haute (forcée)"), CT_ItemDrawableConfiguration::Double, 100 );
    item.addNewConfiguration(staticInitConfigReductionCoef(), QObject::tr("Coef. de reduction"), CT_ItemDrawableConfiguration::Double, 1);
    item.addNewConfiguration(staticInitConfigEnableTransparency(), QObject::tr("Activer transparence"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(staticInitConfigTransparencyValue(), QObject::tr("Valeur de transparence"), CT_ItemDrawableConfiguration::Int, 100);
    item.addNewConfiguration(staticInitConfigXinf(), QObject::tr("Nb. Plans masqués X-"), CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigXsup(), QObject::tr("Nb. Plans masqués X+"), CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigYinf(), QObject::tr("Nb. Plans masqués Y-"), CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigYsup(), QObject::tr("Nb. Plans masqués Y+"), CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigZinf(), QObject::tr("Nb. Plans masqués Z-"), CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigZsup(), QObject::tr("Nb. Plans masqués Z+"), CT_ItemDrawableConfiguration::Int, 0);

    return item;
}

// PROTECTED //
template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigWDisplayValue()
{
    return "A4DGD_WDV";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigWireModeEnabled()
{
    return "A4DGD_WME";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigLowThresholdsEnabled()
{
    return "A4DGD_LTE";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigHighThresholdsEnabled()
{
    return "A4DGD_HTE";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigLowThresholdValue()
{
    return "A4DGD_LTV";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigHighThresholdValue()
{
    return "A4DGD_HTV";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigReductionCoef()
{
    return "A4DGD_RDC";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigEnableTransparency()
{
    return "A4DGD_ETR";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigTransparencyValue()
{
    return "A4DGD_TRV";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigXinf()
{
    return "A4DGD_XIN";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigXsup()
{
    return "A4DGD_XSU";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigYinf()
{
    return "A4DGD_YIN";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigYsup()
{
    return "A4DGD_YSU";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigZinf()
{
    return "A4DGD_ZIN";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigZsup()
{
    return "A4DGD_ZSU";
}

template< typename DataT >
QString CT_StandardGrid4DDrawManager<DataT>::staticInitConfigShowTrueOnly()
{
    return "A4DGD_STO";
}


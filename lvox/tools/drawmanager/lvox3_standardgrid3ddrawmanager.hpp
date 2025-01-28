#include "tools/drawmanager/lvox3_standardgrid3ddrawmanager.h"
#include "tools/3dgrid/lvox3_grid3d.h"
#include "painterinterface.h"

#include <typeinfo>
#include <QObject>
#include <QOpenGLFunctions>
#include <QDebug>

template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_WIRE_MODE_ENABLED = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigWireModeEnabled();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_LOW_THRESHOLDS_ENABLED = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigLowThresholdsEnabled();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_HIGH_THRESHOLDS_ENABLED = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigHighThresholdsEnabled();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_LOW_THRESHOLDS_VALUE = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigLowThresholdValue();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_HIGH_THRESHOLDS_VALUE = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigHighThresholdValue();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_REDUCTION_COEF = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigReductionCoef();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_USE_PREDEFINED_COLORS = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigUsePredefinedColors();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_TRANSPARENCY_VALUE = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigTransparencyValue();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_XINF = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigXinf();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_XSUP = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigXsup();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_YINF = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigYinf();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_YSUP = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigYsup();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_ZINF = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigZinf();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_ZSUP = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigZsup();
template< typename DataT > const QString LVOX3_StandardGrid3DDrawManager<DataT>::INDEX_CONFIG_SHOW_TRUES_ONLY = LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigShowTrueOnly();

template< typename DataT >
LVOX3_StandardGrid3DDrawManager<DataT>::LVOX3_StandardGrid3DDrawManager(QString drawConfigurationName)
    : CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager(drawConfigurationName.isEmpty() ? LVOX3_Grid3D<DataT>::staticName() : drawConfigurationName)
{
    
}

template< typename DataT >
LVOX3_StandardGrid3DDrawManager<DataT>::~LVOX3_StandardGrid3DDrawManager()
{
}

template< typename DataT >
void LVOX3_StandardGrid3DDrawManager<DataT>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::draw(view, painter, itemDrawable);

    const LVOX3_Grid3D<DataT> &item = dynamic_cast<const LVOX3_Grid3D<DataT>&>(itemDrawable);

    // Getting the configuration values
    GLenum  drawingMode;
    bool    wireMode = drawConfiguration()->variableValue(INDEX_CONFIG_WIRE_MODE_ENABLED).toBool();
    bool    uselowThresh = drawConfiguration()->variableValue(INDEX_CONFIG_LOW_THRESHOLDS_ENABLED).toBool();
    bool    usehighThresh = drawConfiguration()->variableValue(INDEX_CONFIG_HIGH_THRESHOLDS_ENABLED).toBool();
    double  lowThresh = drawConfiguration()->variableValue(INDEX_CONFIG_LOW_THRESHOLDS_VALUE).toDouble();
    double  highThresh = drawConfiguration()->variableValue(INDEX_CONFIG_HIGH_THRESHOLDS_VALUE).toDouble();
    double  reductionCoef = drawConfiguration()->variableValue(INDEX_CONFIG_REDUCTION_COEF).toDouble();
    int     transparencyValue = drawConfiguration()->variableValue(INDEX_CONFIG_TRANSPARENCY_VALUE).toInt();
    bool    usePredefinedColors = drawConfiguration()->variableValue(INDEX_CONFIG_USE_PREDEFINED_COLORS).toBool();

    size_t     nXinf = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_XINF).toInt();
    size_t     nXsup = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_XSUP).toInt();
    size_t     nYinf = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_YINF).toInt();
    size_t     nYsup = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_YSUP).toInt();
    size_t     nZinf = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_ZINF).toInt();
    size_t     nZsup = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_ZSUP).toInt();

    if (nXsup > item.xdim()) {nXsup = item.xdim();}
    if (nYsup > item.ydim()) {nXsup = item.ydim();}
    if (nZsup > item.zdim()) {nXsup = item.zdim();}

    if (transparencyValue > 255) {transparencyValue = 255;}
    if (transparencyValue < 0) {transparencyValue = 0;}

    if ( wireMode ) {drawingMode = GL_LINE;}
    else            {drawingMode = GL_FILL;}

    if (!uselowThresh) {lowThresh =  (double)item.dataMin();}
    if (!usehighThresh) {highThresh =  (double)item.dataMax();}

    bool drawAsMap = true;
    if (lowThresh == highThresh) {drawAsMap = false;} // Si les limites sont égales : une seule couleur

    QColor color = painter.getColor();

    // For each voxel of the grid
    double scaling = 240.0 / (highThresh - lowThresh);
    double offset = - (240*lowThresh)/(highThresh - lowThresh);

    size_t xdim = item.xdim();
    size_t ydim = item.ydim();
    size_t zdim = item.zdim();
    double xdemiRes = reductionCoef*item.xresolution() / 2.0;
    double ydemiRes = reductionCoef*item.yresolution() / 2.0;
    double zdemiRes = reductionCoef*item.zresolution() / 2.0;

    double xmin, ymin, zmin, xmax, ymax, zmax;

    // For each voxel of the grid
    for (size_t xx = nXinf ; xx < (xdim - nXsup) ; xx++)
    {
        for (size_t yy = nYinf ; yy < (ydim - nYsup)  ; yy++)
        {
            for (size_t zz = nZinf ; zz < (zdim - nZsup); zz++)
            {
                size_t index;
                if (item.index(xx, yy, zz, index))
                {
                    DataT data = item.valueAtIndex(index);

                    // Draw a cube if the value it contains is between the two thresholds
                    if ( data >= lowThresh && data <= highThresh )
                    {
                        bool predef = false;
                        if (usePredefinedColors && item.colorsDefined())
                        {
                            painter.setColor(item.getColorForValue(data));
                            predef = true;
                        }

                        if (!predef && drawAsMap && !itemDrawable.isSelected())
                        {
                            double h = (int) qRound((data*scaling) + offset);
                            painter.setColor( QColor::fromHsv(h,255,255,transparencyValue) );
                        } else if (!predef){
                            painter.setColor(QColor(255,255,255, transparencyValue));
                        }

                        xmin = item.getCellCenterX(xx) - xdemiRes;
                        ymin = item.getCellCenterY(yy) - ydemiRes;
                        zmin = item.getCellCenterZ(zz) - zdemiRes;
                        xmax = item.getCellCenterX(xx) + xdemiRes;
                        ymax = item.getCellCenterY(yy) + ydemiRes;
                        zmax = item.getCellCenterZ(zz) + zdemiRes;

                        painter.drawCube( xmin, ymin, zmin, xmax, ymax, zmax, GL_FRONT_AND_BACK, drawingMode );
                    }
                } else {
                    qDebug() << "Problème d'index (drawmanager)";
                }
            }
        }
    }
    painter.setColor(color);
}

template< typename DataT >
CT_ItemDrawableConfiguration LVOX3_StandardGrid3DDrawManager<DataT>::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addNewConfiguration(staticInitConfigWireModeEnabled(), QObject::tr("Mode filaire"), CT_ItemDrawableConfiguration::Bool, true);             // Draw the grid in wire mode
    item.addNewConfiguration(staticInitConfigLowThresholdsEnabled(), QObject::tr("Forcer limite basse"), CT_ItemDrawableConfiguration::Bool, true);     // Using thresholds or not
    item.addNewConfiguration(staticInitConfigLowThresholdValue(), QObject::tr("Limite basse (forcée)"), CT_ItemDrawableConfiguration::Double, 1 );         // Voxels with value lesser than this threshold will not be drawn
    item.addNewConfiguration(staticInitConfigHighThresholdsEnabled(), QObject::tr("Forcer limite haute"), CT_ItemDrawableConfiguration::Bool, false);     // Using thresholds or not
    item.addNewConfiguration(staticInitConfigHighThresholdValue(), QObject::tr("Limite haute (forcée)"), CT_ItemDrawableConfiguration::Double, 100 );        // Voxels with value greater than this threshold will not be drawn
    item.addNewConfiguration(staticInitConfigReductionCoef(), QObject::tr("Coef. de reduction"), CT_ItemDrawableConfiguration::Double, 1);
    item.addNewConfiguration(staticInitConfigUsePredefinedColors(), QObject::tr("Utiliser couleurs pré-définies"), CT_ItemDrawableConfiguration::Bool, true);             // Draw the grid with pre-defined colors
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
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigWireModeEnabled()
{
    return "A3DGD_WME";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigLowThresholdsEnabled()
{
    return "A3DGD_LTE";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigHighThresholdsEnabled()
{
    return "A3DGD_HTE";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigLowThresholdValue()
{
    return "A3DGD_LTV";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigHighThresholdValue()
{
    return "A3DGD_HTV";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigReductionCoef()
{
    return "A3DGD_RDC";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigUsePredefinedColors()
{
    return "A3DGD_PDC";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigTransparencyValue()
{
    return "A3DGD_TRV";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigXinf()
{
    return "A3DGD_XIN";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigXsup()
{
    return "A3DGD_XSU";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigYinf()
{
    return "A3DGD_YIN";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigYsup()
{
    return "A3DGD_YSU";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigZinf()
{
    return "A3DGD_ZIN";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigZsup()
{
    return "A3DGD_ZSU";
}

template< typename DataT >
QString LVOX3_StandardGrid3DDrawManager<DataT>::staticInitConfigShowTrueOnly()
{
    return "A3DGD_STO";
}

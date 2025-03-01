#include "lvox3_standardgrid3d_sparsedrawmanager.h"
#include "tools/3dgrid/lvox3_grid3d.h"
#include "painterinterface.h"

#include <typeinfo>
#include <QObject>
#include <QDebug>

// TODO EZ : REMOVE

template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_WIRE_MODE_ENABLED = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigWireModeEnabled();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_LOW_THRESHOLDS_ENABLED = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigLowThresholdsEnabled();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_HIGH_THRESHOLDS_ENABLED = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigHighThresholdsEnabled();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_LOW_THRESHOLDS_VALUE = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigLowThresholdValue();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_HIGH_THRESHOLDS_VALUE = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigHighThresholdValue();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_REDUCTION_COEF = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigReductionCoef();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_USE_PREDEFINED_COLORS = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigUsePredefinedColors();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_TRANSPARENCY_VALUE = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigTransparencyValue();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_XINF = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigXinf();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_XSUP = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigXsup();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_YINF = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigYinf();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_YSUP = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigYsup();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_ZINF = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigZinf();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_HIDE_PLANE_NB_ZSUP = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigZsup();
template< typename DataT > const QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::INDEX_CONFIG_SHOW_TRUES_ONLY = LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigShowTrueOnly();

template< typename DataT >
LVOX3_StandardGrid3D_SparseDrawManager<DataT>::LVOX3_StandardGrid3D_SparseDrawManager(QString drawConfigurationName)
    : CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager(drawConfigurationName.isEmpty() ? LVOX3_Grid3D<DataT>::staticName() : drawConfigurationName)
{
    
}

template< typename DataT >
LVOX3_StandardGrid3D_SparseDrawManager<DataT>::~LVOX3_StandardGrid3D_SparseDrawManager()
{
}

template< typename DataT >
void LVOX3_StandardGrid3D_SparseDrawManager<DataT>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
#ifdef USE_OPENCV
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::draw(view, painter, itemDrawable);

    const LVOX3_Grid3D_Sparse<DataT> &item = dynamic_cast<const LVOX3_Grid3D_Sparse<DataT>&>(itemDrawable);

    // Getting the configuration values
    GLenum  drawingMode;
    bool    wireMode = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_WIRE_MODE_ENABLED).toBool();
    bool    uselowThresh = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_LOW_THRESHOLDS_ENABLED).toBool();
    bool    usehighThresh = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_HIGH_THRESHOLDS_ENABLED).toBool();
    double  lowThresh = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_LOW_THRESHOLDS_VALUE).toDouble();
    double  highThresh = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_HIGH_THRESHOLDS_VALUE).toDouble();
    double  reductionCoef = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_REDUCTION_COEF).toDouble();
    int     transparencyValue = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_TRANSPARENCY_VALUE).toInt();
    bool    usePredefinedColors = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_USE_PREDEFINED_COLORS).toBool();

    size_t     nXinf = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_HIDE_PLANE_NB_XINF).toInt();
    size_t     nXsup = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_HIDE_PLANE_NB_XSUP).toInt();
    size_t     nYinf = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_HIDE_PLANE_NB_YINF).toInt();
    size_t     nYsup = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_HIDE_PLANE_NB_YSUP).toInt();
    size_t     nZinf = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_HIDE_PLANE_NB_ZINF).toInt();
    size_t     nZsup = this->getDrawConfiguration()->getVariableValue(INDEX_CONFIG_HIDE_PLANE_NB_ZSUP).toInt();

    size_t xinf = nXinf;
    size_t yinf = nYinf;
    size_t zinf = nZinf;
    size_t xsup = 0;
    size_t ysup = 0;
    size_t zsup = 0;

    if (nXsup < item.xdim()) {xsup = item.xdim() - nXsup - 1;}
    if (nYsup < item.ydim()) {ysup = item.ydim() - nYsup - 1;}
    if (nZsup < item.zdim()) {zsup = item.zdim() - nZsup - 1;}


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

    double demiResX = reductionCoef*item.xresolution() / 2.0;
    double demiResY = reductionCoef*item.yresolution() / 2.0;
    double demiResZ = reductionCoef*item.zresolution() / 2.0;

    double xmin, ymin, zmin, xmax, ymax, zmax;
    size_t xx, yy, zz;

    // For each voxel of the grid
    QList<size_t> indices;
    item.getIndicesWithData(indices);

    for (int i = 0 ; i < indices.size() ; i++)
    {
        size_t index = indices.at(i);

        item.indexToGrid(index, xx, yy, zz);

        if( xx >= xinf && xx <= xsup && yy >= yinf && yy <= ysup && zz >= zinf && zz <= zsup )
        {           
            DataT data = item.valueAtIndex(index);

            // Draw a cube if the value it contains is between the two thresholds
            if ( data != item.NA() && data >= lowThresh && data <= highThresh )
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

                xmin = item.getCellCenterX(xx) - demiResX;
                ymin = item.getCellCenterY(yy) - demiResY;
                zmin = item.getCellCenterZ(zz) - demiResZ;
                xmax = item.getCellCenterX(xx) + demiResX;
                ymax = item.getCellCenterY(yy) + demiResY;
                zmax = item.getCellCenterZ(zz) + demiResZ;

                painter.drawCube( xmin, ymin, zmin, xmax, ymax, zmax, GL_FRONT_AND_BACK, drawingMode );
            }
        }
    }

    painter.setColor(color);
#endif
}

template< typename DataT >
CT_ItemDrawableConfiguration LVOX3_StandardGrid3D_SparseDrawManager<DataT>::createDrawConfiguration(QString drawConfigurationName) const
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
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigWireModeEnabled()
{
    return "A3DSPGD_WME";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigLowThresholdsEnabled()
{
    return "A3DSPGD_LTE";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigHighThresholdsEnabled()
{
    return "A3DSPGD_HTE";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigLowThresholdValue()
{
    return "A3DSPGD_LTV";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigHighThresholdValue()
{
    return "A3DSPGD_HTV";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigReductionCoef()
{
    return "A3DSPGD_RDC";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigUsePredefinedColors()
{
    return "A3DSPGD_PDC";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigTransparencyValue()
{
    return "A3DSPGD_TRV";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigXinf()
{
    return "A3DSPGD_XIN";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigXsup()
{
    return "A3DSPGD_XSU";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigYinf()
{
    return "A3DSPGD_YIN";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigYsup()
{
    return "A3DSPGD_YSU";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigZinf()
{
    return "A3DSPGD_ZIN";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigZsup()
{
    return "A3DSPGD_ZSU";
}

template< typename DataT >
QString LVOX3_StandardGrid3D_SparseDrawManager<DataT>::staticInitConfigShowTrueOnly()
{
    return "A3DSPGD_STO";
}

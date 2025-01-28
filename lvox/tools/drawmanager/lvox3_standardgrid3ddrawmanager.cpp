#include "lvox3_standardgrid3ddrawmanager.h"
#include "painterinterface.h"

/////////////////////////////////////////////////////////////////////
/// Specialisations for bool type ///////////////////////////////////
/////////////////////////////////////////////////////////////////////

template<>
void LVOX3_StandardGrid3DDrawManager<bool>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::draw(view, painter, itemDrawable);

    const LVOX3_Grid3D<bool> &item = dynamic_cast<const LVOX3_Grid3D<bool>&>(itemDrawable);

    // Getting the configuration values
    GLenum  drawingMode;
    bool    wireMode = drawConfiguration()->variableValue(INDEX_CONFIG_WIRE_MODE_ENABLED).toBool();
    double  reductionCoef = drawConfiguration()->variableValue(INDEX_CONFIG_REDUCTION_COEF).toDouble();
    int     transparencyValue = drawConfiguration()->variableValue(INDEX_CONFIG_TRANSPARENCY_VALUE).toInt();
    bool    showTrueOnly = drawConfiguration()->variableValue(INDEX_CONFIG_SHOW_TRUES_ONLY).toBool();

    size_t     nXinf = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_XINF).toUInt();
    size_t     nXsup = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_XSUP).toUInt();
    size_t     nYinf = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_YINF).toUInt();
    size_t     nYsup = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_YSUP).toUInt();
    size_t     nZinf = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_ZINF).toUInt();
    size_t     nZsup = drawConfiguration()->variableValue(INDEX_CONFIG_HIDE_PLANE_NB_ZSUP).toUInt();

    //VTNguyen: size_t is always >=0
    //if (nXinf < 0) {nXinf = 0;}
    //if (nYinf < 0) {nYinf = 0;}
    //if (nZinf < 0) {nZinf = 0;}

    if (nXsup > item.xdim()) {nXsup = item.xdim();}
    if (nYsup > item.ydim()) {nXsup = item.ydim();}
    if (nZsup > item.zdim()) {nXsup = item.zdim();}

    if (transparencyValue > 255) {transparencyValue = 255;}
    if (transparencyValue < 0) {transparencyValue = 0;}

    if ( wireMode ) {drawingMode = GL_LINE;}
    else            {drawingMode = GL_FILL;}

    bool drawAsMap = true;

    QColor color = painter.getColor();

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
                    bool data = item.valueAtIndex(index);

                    if (data || !showTrueOnly)
                    {
                        if (drawAsMap && !itemDrawable.isSelected())
                        {
                            if (data)
                            {
                                painter.setColor(QColor(0,255,0, transparencyValue));
                            } else {
                                painter.setColor(QColor(255,0,0, transparencyValue));
                            }


                            xmin = item.getCellCenterX(xx) - xdemiRes;
                            ymin = item.getCellCenterY(yy) - ydemiRes;
                            zmin = item.getCellCenterZ(zz) - zdemiRes;
                            xmax = item.getCellCenterX(xx) + xdemiRes;
                            ymax = item.getCellCenterY(yy) + ydemiRes;
                            zmax = item.getCellCenterZ(zz) + zdemiRes;

                            painter.drawCube( xmin, ymin, zmin, xmax, ymax, zmax, GL_FRONT_AND_BACK, drawingMode );
                        }
                    }
                } else {
                    qDebug() << "Problème d'index (drawmanager)";
                }
            }
        }
        painter.setColor(color);
    }
}

template<>
CT_ItemDrawableConfiguration LVOX3_StandardGrid3DDrawManager<bool>::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addNewConfiguration(staticInitConfigWireModeEnabled(), "Mode filaire", CT_ItemDrawableConfiguration::Bool, true);             // Draw the grid in wire mode
    item.addNewConfiguration(staticInitConfigReductionCoef(), "Coef. de reduction", CT_ItemDrawableConfiguration::Double, 1);
    item.addNewConfiguration(staticInitConfigShowTrueOnly(), "Valeurs TRUE seulement", CT_ItemDrawableConfiguration::Bool, 1);
    item.addNewConfiguration(staticInitConfigTransparencyValue(), "Valeur de transparence", CT_ItemDrawableConfiguration::Int, 100);
    item.addNewConfiguration(staticInitConfigXinf(), "Nb. Plans masqués X-", CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigXsup(), "Nb. Plans masqués X+", CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigYinf(), "Nb. Plans masqués Y-", CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigYsup(), "Nb. Plans masqués Y+", CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigZinf(), "Nb. Plans masqués Z-", CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigZsup(), "Nb. Plans masqués Z+", CT_ItemDrawableConfiguration::Int, 0);

    return item;
}


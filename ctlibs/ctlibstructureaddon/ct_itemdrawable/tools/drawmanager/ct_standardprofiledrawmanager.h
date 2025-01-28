#ifndef CT_STANDARDPROFILEDRAWMANAGER_H
#define CT_STANDARDPROFILEDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"
#include "ct_itemdrawable/ct_profile.h"

template< typename DataT>
class CT_StandardProfileDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardProfileDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_COLUMN_MODE_ENABLED;
    const static QString INDEX_CONFIG_COLUMN_RADIUS;
    const static QString INDEX_CONFIG_GRAPH_MODE_ENABLED;
    const static QString INDEX_CONFIG_DRAW_AXIS;
    const static QString INDEX_CONFIG_DRAW_BARS;
    const static QString INDEX_CONFIG_DRAW_POINTS;
    const static QString INDEX_CONFIG_POINTS_SIZE;
    const static QString INDEX_CONFIG_DRAW_CURVE;
    const static QString INDEX_CONFIG_GRAPH_ORIENTATION;
    const static QString INDEX_CONFIG_SCALE;
    const static QString INDEX_CONFIG_SCALE8COEFFICIENT;

    static QString staticInitConfigColumnModeEnabled();
    static QString staticInitConfigColumnRadius();
    static QString staticInitConfigGraphModeEnabled();
    static QString staticInitConfigDrawAxis();
    static QString staticInitConfigDrawBars();
    static QString staticInitConfigDrawPoints();
    static QString staticInitConfigPointsSize();
    static QString staticInitConfigDrawCurve();
    static QString staticInitConfigGraphOrientation();
    static QString staticInitConfigScale();
    static QString staticInitConfigScaleCoeff();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#include "ct_itemdrawable/tools/drawmanager/ct_standardprofiledrawmanager.hpp"

#endif // CT_STANDARDPROFILEDRAWMANAGER_H

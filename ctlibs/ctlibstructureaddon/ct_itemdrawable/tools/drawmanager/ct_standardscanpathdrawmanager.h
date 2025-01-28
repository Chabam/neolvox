#ifndef CT_STANDARDSCANPATHMANAGER_H
#define CT_STANDARDSCANPATHMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardScanPathDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardScanPathDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_DRAW_POINTS;
    const static QString INDEX_CONFIG_DRAW_LINES;
    const static QString INDEX_CONFIG_POINT_SIZE;

    static QString staticInitConfigDrawPoints();
    static QString staticInitConfigDrawLines();
    static QString staticInitConfigPointSize();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDSCANPATHMANAGER_H

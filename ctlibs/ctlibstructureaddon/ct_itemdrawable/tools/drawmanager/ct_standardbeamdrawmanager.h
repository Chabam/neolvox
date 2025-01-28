#ifndef CT_STANDARDBEAMDRAWMANAGER_H
#define CT_STANDARDBEAMDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardBeamDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardBeamDrawManager(const QString& drawConfigurationName = "");

    void draw(GraphicsViewInterface& view, PainterInterface& painter, const CT_AbstractItemDrawable& itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_LENGTH_OF_BEAM;

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;

    static QString staticInitConfigLengthOfRay();
};

#endif // CT_STANDARDBEAMDRAWMANAGER_H

#ifndef CT_STANDARDABSTRACTSHAPE2DDRAWMANAGER_H
#define CT_STANDARDABSTRACTSHAPE2DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardAbstractShape2DDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardAbstractShape2DDrawManager(QString drawConfigurationName = "");

    void setZValue(double val);

protected:
    static const QString INDEX_CONFIG_USE_ALTERNATIVE_ZVALUE;
    static const QString INDEX_CONFIG_Z_VALUE;

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;

    static QString staticInitConfigUseAlternativeZValue();
    static QString staticInitConfigZValue();
};

#endif // CT_STANDARDABSTRACTSHAPE2DDRAWMANAGER_H

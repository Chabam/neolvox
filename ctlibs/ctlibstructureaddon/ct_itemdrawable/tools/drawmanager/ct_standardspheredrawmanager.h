#ifndef CT_STANDARDSPHEREDRAWMANAGER_H
#define CT_STANDARDSPHEREDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

#include <QObject>

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardSphereDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardSphereDrawManager(QString drawConfigurationName = QObject::tr("Sphere"));

    void draw(GraphicsViewInterface& view,
              PainterInterface& painter,
              const CT_AbstractItemDrawable& itemDrawable) const override;
};

#endif // CT_STANDARDSPHEREDRAWMANAGER_H

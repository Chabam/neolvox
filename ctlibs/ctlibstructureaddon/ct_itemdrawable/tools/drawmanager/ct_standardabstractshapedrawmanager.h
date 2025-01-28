#ifndef CT_STANDARDABSTRACTSHAPEDRAWMANAGER_H
#define CT_STANDARDABSTRACTSHAPEDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardAbstractShapeDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardAbstractShapeDrawManager(QString drawConfigurationName = "");
};

#endif // CT_STANDARDABSTRACTSHAPEDRAWMANAGER_H

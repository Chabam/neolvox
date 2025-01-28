#ifndef CT_HANDLEINPOINTSCALARMASK_H
#define CT_HANDLEINPOINTSCALARMASK_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_pointsattributesscalarmaskt.h"

template<class StructType, int min = 1, int max = 1>
class CT_HandleInPointScalarMask : public CT_HandleInSingularItem<CT_PointsAttributesScalarMaskT<StructType>, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleInPointScalarMask() = default;
};

#endif // CT_HANDLEINPOINTSCALARMASK_H

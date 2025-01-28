#ifndef CT_HANDLEINPOINTCOLOR_H
#define CT_HANDLEINPOINTCOLOR_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_pointsattributescolor.h"

template<int min = 1, int max = 1>
class CT_HandleInPointColor : public CT_HandleInSingularItem<CT_PointsAttributesColor, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleInPointColor() = default;
};

#endif // CT_HANDLEINPOINTCOLOR_H

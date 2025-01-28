#ifndef CT_HANDLEINPOINTNORMAL_H
#define CT_HANDLEINPOINTNORMAL_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_pointsattributesnormal.h"

template<int min = 1, int max = 1>
class CT_HandleInPointNormal : public CT_HandleInSingularItem<CT_PointsAttributesNormal, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleInPointNormal() = default;
};

#endif // CT_HANDLEINPOINTNORMAL_H

#ifndef CT_HANDLEINEDGECOLOR_H
#define CT_HANDLEINEDGECOLOR_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_edgeattributescolor.h"

template<int min = 1, int max = 1>
class CT_HandleInEdgeColor : public CT_HandleInSingularItem<CT_EdgeAttributesColor, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToEdge = 1
    };

    CT_HandleInEdgeColor() = default;
};

#endif // CT_HANDLEINEDGECOLOR_H

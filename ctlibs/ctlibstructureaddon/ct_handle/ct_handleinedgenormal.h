#ifndef CT_HANDLEINEDGENORMAL_H
#define CT_HANDLEINEDGENORMAL_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_edgeattributesnormal.h"

template<int min = 1, int max = 1>
class CT_HandleInEdgeNormal : public CT_HandleInSingularItem<CT_EdgeAttributesNormal, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToEdge = 1
    };

    CT_HandleInEdgeNormal() = default;
};

#endif // CT_HANDLEINEDGENORMAL_H

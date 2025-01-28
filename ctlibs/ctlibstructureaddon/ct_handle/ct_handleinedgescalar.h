#ifndef CT_HANDLEINEDGESCALAR_H
#define CT_HANDLEINEDGESCALAR_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_edgeattributesscalart.h"

template<class SCALAR, int min = 1, int max = 1>
class CT_HandleInEdgecalar : public CT_HandleInSingularItem<CT_EdgeAttributesScalarT<SCALAR>, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToEdge = 1
    };

    CT_HandleInEdgecalar() = default;
};

template<int min = 1, int max = 1>
class CT_HandleInAbstractEdgecalar : public CT_HandleInSingularItem<CT_AbstractEdgeAttributesScalar, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToEdge = 1
    };

    CT_HandleInAbstractEdgecalar() = default;
};

#endif // CT_HANDLEINEDGESCALAR_H

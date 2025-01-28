#ifndef CT_HANDLEINPOINTSCALAR_H
#define CT_HANDLEINPOINTSCALAR_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_pointsattributesscalartemplated.h"

template<class SCALAR, int min = 1, int max = 1>
class CT_HandleInPointScalar : public CT_HandleInSingularItem<CT_PointsAttributesScalarTemplated<SCALAR>, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleInPointScalar() = default;
};

template<int min = 1, int max = 1>
class CT_HandleInAbstractPointScalar : public CT_HandleInSingularItem<CT_AbstractPointAttributesScalar, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleInAbstractPointScalar() = default;
};

template<int min = 1, int max = 1>
class CT_HandleInAbstractPointAttribute : public CT_HandleInSingularItem<CT_AbstractPointsAttributes, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleInAbstractPointAttribute() = default;
};

#endif // CT_HANDLEINPOINTSCALAR_H

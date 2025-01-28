#ifndef CT_HANDLEOUTPOINTSCALAR_H
#define CT_HANDLEOUTPOINTSCALAR_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_pointsattributesscalartemplated.h"
#include "ct_attributes/managers/ct_densepointscalarmanager.h"
#include "ct_attributes/managers/ct_sparsepointscalarmanager.h"

template<typename SCALAR>
class CT_HandleOutPointScalar : public CT_HandleOutItem<CT_PointsAttributesScalarTemplated<SCALAR>>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleOutPointScalar() = default;
};

template<typename SCALAR>
class CT_HandleOutPointScalarWithDenseManager : public CT_HandleOutPointScalar<SCALAR>, public CT_DensePointScalarManager<SCALAR>
{
public:
    CT_HandleOutPointScalarWithDenseManager() = default;

    CT_PointsAttributesScalarTemplated<SCALAR>* createAttributeInstance(CT_PCIR pcir)
    {
        return new CT_PointsAttributesScalarTemplated<SCALAR>(pcir, *this);
    }

    CT_PointsAttributesScalarTemplated<SCALAR>* createAttributeInstance(CT_PCIR pcir,
                                                                        const SCALAR& min,
                                                                        const SCALAR& max)
    {
        return new CT_PointsAttributesScalarTemplated<SCALAR>(pcir, *this, min, max);
    }
};

template<typename SCALAR>
class CT_HandleOutPointScalarWithSparseManager : public CT_HandleOutPointScalar<SCALAR>, public CT_SparsePointScalarManager<SCALAR>
{
public:
    CT_HandleOutPointScalarWithSparseManager() = default;

    CT_PointsAttributesScalarTemplated<SCALAR>* createAttributeInstance(CT_PCIR pcir)
    {
        return new CT_PointsAttributesScalarTemplated<SCALAR>(pcir, *this);
    }

    CT_PointsAttributesScalarTemplated<SCALAR>* createAttributeInstance(CT_PCIR pcir,
                                                                        const SCALAR& min,
                                                                        const SCALAR& max)
    {
        return new CT_PointsAttributesScalarTemplated<SCALAR>(pcir, *this, min, max);
    }
};

#endif // CT_HANDLEOUTPOINTSCALAR_H

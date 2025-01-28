#ifndef CT_HANDLEOUTFACESCALAR_H
#define CT_HANDLEOUTFACESCALAR_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_faceattributesscalart.h"
#include "ct_attributes/managers/ct_densefacescalarmanager.h"
#include "ct_attributes/managers/ct_sparsefacescalarmanager.h"

template<typename SCALAR>
class CT_HandleOutFaceScalar : public CT_HandleOutItem<CT_FaceAttributesScalarT<SCALAR>>
{
public:
    enum EApplicableTo
    {
        ApplicableToFace = 1
    };

    CT_HandleOutFaceScalar() = default;
};

template<typename SCALAR>
class CT_HandleOutFaceScalarWithDenseManager : public CT_HandleOutFaceScalar<SCALAR>, public CT_DenseFaceScalarManager<SCALAR>
{
public:
    CT_HandleOutFaceScalarWithDenseManager() = default;

    CT_FaceAttributesScalarT<SCALAR>* createAttributeInstance(CT_FCIR fcir)
    {
        return new CT_FaceAttributesScalarT<SCALAR>(fcir, *this);
    }

    CT_FaceAttributesScalarT<SCALAR>* createAttributeInstance(CT_FCIR fcir,
                                                              const SCALAR& min,
                                                              const SCALAR& max)
    {
        return new CT_FaceAttributesScalarT<SCALAR>(fcir, *this, min, max);
    }
};

template<typename SCALAR>
class CT_HandleOutFaceScalarWithSparseManager : public CT_HandleOutFaceScalar<SCALAR>, public CT_SparseFaceScalarManager<SCALAR>
{
public:
    CT_HandleOutFaceScalarWithSparseManager() = default;

    CT_FaceAttributesScalarT<SCALAR>* createAttributeInstance(CT_FCIR fcir)
    {
        return new CT_FaceAttributesScalarT<SCALAR>(fcir, *this);
    }

    CT_FaceAttributesScalarT<SCALAR>* createAttributeInstance(CT_FCIR fcir,
                                                              const SCALAR& min,
                                                              const SCALAR& max)
    {
        return new CT_FaceAttributesScalarT<SCALAR>(fcir, *this, min, max);
    }
};

#endif // CT_HANDLEOUTFACESCALAR_H

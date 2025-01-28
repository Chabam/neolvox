#ifndef CT_HANDLEINFACESCALAR_H
#define CT_HANDLEINFACESCALAR_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_faceattributesscalart.h"

template<class SCALAR, int min = 1, int max = 1>
class CT_HandleInFacecalar : public CT_HandleInSingularItem<CT_FaceAttributesScalarT<SCALAR>, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToFace = 1
    };

    CT_HandleInFacecalar() = default;
};

template<int min = 1, int max = 1>
class CT_HandleInAbstractFacecalar : public CT_HandleInSingularItem<CT_AbstractFaceAttributesScalar, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToFace = 1
    };

    CT_HandleInAbstractFacecalar() = default;
};

#endif // CT_HANDLEINFACESCALAR_H

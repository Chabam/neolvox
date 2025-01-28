#ifndef CT_HANDLEOUTFACENORMAL_H
#define CT_HANDLEOUTFACENORMAL_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_faceattributesnormal.h"
#include "ct_attributes/managers/ct_densefacenormalmanager.h"
#include "ct_attributes/managers/ct_sparsefacenormalmanager.h"

class CT_HandleOutFaceNormal : public CT_HandleOutItem<CT_FaceAttributesNormal>
{
public:
    enum EApplicableTo
    {
        ApplicableToFace = 1
    };

    CT_HandleOutFaceNormal() = default;
};

class CT_HandleOutFaceNormalWithDenseManager : public CT_HandleOutFaceNormal, public CT_DenseFaceNormalManager
{
public:
    CT_HandleOutFaceNormalWithDenseManager() = default;

    CT_FaceAttributesNormal* createAttributeInstance(CT_FCIR fcir)
    {
        return new CT_FaceAttributesNormal(fcir, *this);
    }
};

class CT_HandleOutFaceNormalWithSparseManager : public CT_HandleOutFaceNormal, public CT_SparseFaceNormalManager
{
public:
    CT_HandleOutFaceNormalWithSparseManager() = default;

    CT_FaceAttributesNormal* createAttributeInstance(CT_FCIR fcir)
    {
        return new CT_FaceAttributesNormal(fcir, *this);
    }
};

#endif // CT_HANDLEOUTFACENORMAL_H

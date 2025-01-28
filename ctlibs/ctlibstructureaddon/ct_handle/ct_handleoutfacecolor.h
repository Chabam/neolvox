#ifndef CT_HANDLEOUTFACECOLOR_H
#define CT_HANDLEOUTFACECOLOR_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_faceattributescolor.h"
#include "ct_attributes/managers/ct_densefacecolormanager.h"
#include "ct_attributes/managers/ct_sparsefacecolormanager.h"

class CT_HandleOutFaceColor : public CT_HandleOutItem<CT_FaceAttributesColor>
{
public:
    enum EApplicableTo
    {
        ApplicableToFace = 1
    };

    CT_HandleOutFaceColor() = default;
};

class CT_HandleOutFaceColorWithDenseManager : public CT_HandleOutFaceColor, public CT_DenseFaceColorManager
{
public:
    CT_HandleOutFaceColorWithDenseManager() = default;

    CT_FaceAttributesColor* createAttributeInstance(CT_FCIR fcir)
    {
        return new CT_FaceAttributesColor(fcir, *this);
    }
};

class CT_HandleOutFaceColorWithSparseManager : public CT_HandleOutFaceColor, public CT_SparseFaceColorManager
{
public:
    CT_HandleOutFaceColorWithSparseManager() = default;

    CT_FaceAttributesColor* createAttributeInstance(CT_FCIR fcir)
    {
        return new CT_FaceAttributesColor(fcir, *this);
    }
};

#endif // CT_HANDLEOUTFACECOLOR_H

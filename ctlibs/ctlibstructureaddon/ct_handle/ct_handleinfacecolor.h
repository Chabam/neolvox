#ifndef CT_HANDLEINFACECOLOR_H
#define CT_HANDLEINFACECOLOR_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_faceattributescolor.h"

template<int min = 1, int max = 1>
class CT_HandleInFaceColor : public CT_HandleInSingularItem<CT_FaceAttributesColor, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToFace = 1
    };

    CT_HandleInFaceColor() = default;
};

#endif // CT_HANDLEINFACECOLOR_H

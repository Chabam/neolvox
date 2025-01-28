#ifndef CT_HANDLEINFACENORMAL_H
#define CT_HANDLEINFACENORMAL_H

#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_itemdrawable/ct_faceattributesnormal.h"

template<int min = 1, int max = 1>
class CT_HandleInFaceNormal : public CT_HandleInSingularItem<CT_FaceAttributesNormal, min, max>
{
public:
    enum EApplicableTo
    {
        ApplicableToFace = 1
    };

    CT_HandleInFaceNormal() = default;
};

#endif // CT_HANDLEINFACENORMAL_H

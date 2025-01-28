#ifndef CT_HANDLEOUTPOINTSCALARMASK_H
#define CT_HANDLEOUTPOINTSCALARMASK_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_pointsattributesscalarmaskt.h"
#include "ct_attributes/managers/ct_densepointscalarmanager.h"
#include "ct_attributes/managers/ct_sparsepointscalarmanager.h"

template<typename StructType>
class CT_HandleOutPointScalarMask : public CT_HandleOutItem<CT_PointsAttributesScalarMaskT<StructType>>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleOutPointScalarMask() = default;
};

template<typename StructType>
class CT_HandleOutPointScalarMaskWithDenseManager : public CT_HandleOutPointScalarMask<StructType>, public CT_DensePointScalarManager<StructType>
{
public:
    CT_HandleOutPointScalarMaskWithDenseManager() = default;

    CT_PointsAttributesScalarMaskT<StructType>* createAttributeInstance(typename CT_PointsAttributesScalarMaskT<StructType>::StructMASK mask,
                                                                        quint16 shiftRight,
                                                                        CT_PCIR pcir)
    {
        return new CT_PointsAttributesScalarMaskT<StructType>(mask, shiftRight, pcir, *this);
    }

    CT_PointsAttributesScalarMaskT<StructType>* createAttributeInstance(typename CT_PointsAttributesScalarMaskT<StructType>::StructMASK mask,
                                                                        quint16 shiftRight,
                                                                        const typename CT_PointsAttributesScalarMaskT<StructType>::StructMASK &min,
                                                                        const typename CT_PointsAttributesScalarMaskT<StructType>::StructMASK &max,
                                                                        CT_PCIR pcir)
    {
        return new CT_PointsAttributesScalarMaskT<StructType>(mask, shiftRight, min, max, pcir, *this);
    }
};

template<typename StructType>
class CT_HandleOutPointScalarMaskWithSparseManager : public CT_HandleOutPointScalarMask<StructType>, public CT_SparsePointScalarManager<StructType>
{
public:
    CT_HandleOutPointScalarMaskWithSparseManager() = default;
};

#endif // CT_HANDLEOUTPOINTSCALARMASK_H

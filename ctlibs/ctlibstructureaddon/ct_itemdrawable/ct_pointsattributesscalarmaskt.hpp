#include "ct_itemdrawable/ct_pointsattributesscalarmaskt.h"

template<typename StructType>
CT_PointsAttributesScalarMaskT<StructType>::CT_PointsAttributesScalarMaskT() :
    SuperClass(),
    m_mask(StructMASK(0)),
    m_shiftRight(0)
{
}

template<typename StructType>
CT_PointsAttributesScalarMaskT<StructType>::CT_PointsAttributesScalarMaskT(const CT_PointsAttributesScalarMaskT<StructType>& other) :
    SuperClass(other),
    m_mask(other.m_mask),
    m_shiftRight(other.m_shiftRight)
{
}

template<typename StructType>
quint64 CT_PointsAttributesScalarMaskT<StructType>::mask() const
{
    return quint64(m_mask);
}

template<typename StructType>
typename CT_PointsAttributesScalarMaskT<StructType>::StructMASK CT_PointsAttributesScalarMaskT<StructType>::convertScalarOfManagerToScalar(const StructType& value) const
{
    return ((value.entire & m_mask) >> m_shiftRight);
}

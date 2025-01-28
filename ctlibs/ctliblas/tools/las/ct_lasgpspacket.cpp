#include "ct_lasgpspacket.h"

#include "abstract/ct_abstractlaspointformat.h"

CT_LASGpsPacket::CT_LASGpsPacket()
{
}

size_t CT_LASGpsPacket::gpsPacketSizeInBytes() const
{
    return sizeof(m_emptyData);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASGpsPacket::gpsPacketTypesNeeded()
{
    QList<CT_LasDefine::LASPointAttributesType> l;

    l.append(CT_LasDefine::GPS_Time);

    return l;
}

void CT_LASGpsPacket::writeGpsPacket(QDataStream &stream, const size_t& globalIndex, const CT_AbstractPointAttributesScalar* gpsValues) const
{
    CT_WRITE_LAS_SCALAR_WITH(gpsValues, double) // gps time
}


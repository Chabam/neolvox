#include "ct_lazgpspacket.h"

#include "abstract/ct_abstractlazpointformat.h"

#include "laszip/laszip_api.h"

CT_LAZGpsPacket::CT_LAZGpsPacket()
{
}

size_t CT_LAZGpsPacket::gpsPacketSizeInBytes() const
{
    return sizeof(m_emptyData);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZGpsPacket::gpsPacketTypesNeeded()
{
    QList<CT_LasDefine::LASPointAttributesType> l;

    l.append(CT_LasDefine::GPS_Time);

    return l;
}

void CT_LAZGpsPacket::writeGpsPacket(laszip_point* lzPoint, const size_t& globalIndex, const CT_AbstractPointAttributesScalar* gpsValues) const
{
    // CT_WRITE_LAS_SCALAR_WITH(gpsValues, double) // gps time
    CT_WRITE_LAZ_SCALAR_WITH(gpsValues, double, gps_time) // gps time
}


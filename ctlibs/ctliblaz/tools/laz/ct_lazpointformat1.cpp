#include "ct_lazpointformat1.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat1::CT_LAZPointFormat1() : CT_LAZPointFormat0()
{
}

size_t CT_LAZPointFormat1::sizeInBytes() const
{
    return CT_LAZPointFormat0::sizeInBytes() + gpsPacketSizeInBytes();
}

void CT_LAZPointFormat1::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LAZPointFormat0::write(lzPoint, header, p, globalIndex);

    writeInfoFormat1(lzPoint, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat1::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LAZPointFormat0::typesToSearch();
    l.append(gpsPacketTypesNeeded());

    return l;
}

void CT_LAZPointFormat1::writeInfoFormat1(laszip_point* lzPoint, const size_t& globalIndex) const
{
    writeGpsPacket(lzPoint, globalIndex, scalars(CT_LasDefine::GPS_Time));
}

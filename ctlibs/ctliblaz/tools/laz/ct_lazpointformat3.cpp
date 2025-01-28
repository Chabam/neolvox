#include "ct_lazpointformat3.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat3::CT_LAZPointFormat3() : CT_LAZPointFormat2()
{
}

size_t CT_LAZPointFormat3::sizeInBytes() const
{
    return CT_LAZPointFormat2::sizeInBytes() + gpsPacketSizeInBytes();
}

void CT_LAZPointFormat3::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LAZPointFormat0::write(lzPoint, header, p, globalIndex);

    writeInfoFormat3(lzPoint, globalIndex);
    CT_LAZPointFormat2::writeInfoFormat2(lzPoint, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat3::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LAZPointFormat2::typesToSearch();
    l.append(gpsPacketTypesNeeded());

    return l;
}

void CT_LAZPointFormat3::writeInfoFormat3(laszip_point* lzPoint, const size_t &globalIndex) const
{
    writeGpsPacket(lzPoint, globalIndex, scalars(CT_LasDefine::GPS_Time));
}

#include "ct_laspointformat1.h"

CT_LASPointFormat1::CT_LASPointFormat1() : CT_LASPointFormat0()
{
}

size_t CT_LASPointFormat1::sizeInBytes() const
{
    return CT_LASPointFormat0::sizeInBytes() + gpsPacketSizeInBytes();
}

void CT_LASPointFormat1::write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LASPointFormat0::write(stream, header, p, globalIndex);

    writeInfoFormat1(stream, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASPointFormat1::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LASPointFormat0::typesToSearch();
    l.append(gpsPacketTypesNeeded());

    return l;
}

void CT_LASPointFormat1::writeInfoFormat1(QDataStream &stream, const size_t& globalIndex) const
{
    writeGpsPacket(stream, globalIndex, scalars(CT_LasDefine::GPS_Time));
}

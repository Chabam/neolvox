#include "ct_laspointformat3.h"

CT_LASPointFormat3::CT_LASPointFormat3() : CT_LASPointFormat2()
{
}

size_t CT_LASPointFormat3::sizeInBytes() const
{
    return CT_LASPointFormat2::sizeInBytes() + gpsPacketSizeInBytes();
}

void CT_LASPointFormat3::write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LASPointFormat0::write(stream, header, p, globalIndex);

    writeInfoFormat3(stream, globalIndex);
    CT_LASPointFormat2::writeInfoFormat2(stream, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASPointFormat3::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LASPointFormat2::typesToSearch();
    l.append(gpsPacketTypesNeeded());

    return l;
}

void CT_LASPointFormat3::writeInfoFormat3(QDataStream &stream, const size_t& globalIndex) const
{
    writeGpsPacket(stream, globalIndex, scalars(CT_LasDefine::GPS_Time));
}

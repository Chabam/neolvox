#include "ct_lazpointformat8.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat8::CT_LAZPointFormat8() : CT_LAZPointFormat7()
{
}

size_t CT_LAZPointFormat8::sizeInBytes() const
{
    return CT_LAZPointFormat7::sizeInBytes() + nirPacketSizeInBytes();
}

void CT_LAZPointFormat8::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LAZPointFormat7::write(lzPoint, header, p, globalIndex);

    writeInfoFormat8(lzPoint, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat8::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LAZPointFormat7::typesToSearch();
    l.append(nirPacketTypesNeeded());

    return l;
}

void CT_LAZPointFormat8::writeInfoFormat8(laszip_point* lzPoint, const size_t& globalIndex) const
{
    writeNirPacket(lzPoint, globalIndex, scalars(CT_LasDefine::NIR));
}

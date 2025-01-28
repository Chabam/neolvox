#include "ct_lazpointformat2.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat2::CT_LAZPointFormat2() : CT_LAZPointFormat0()
{
}

size_t CT_LAZPointFormat2::sizeInBytes() const
{
    return CT_LAZPointFormat0::sizeInBytes() + colorPacketSizeInBytes();
}

void CT_LAZPointFormat2::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LAZPointFormat0::write(lzPoint, header, p, globalIndex);

    writeInfoFormat2(lzPoint, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat2::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LAZPointFormat0::typesToSearch();
    l.append(colorPacketTypesNeeded());

    return l;
}

void CT_LAZPointFormat2::writeInfoFormat2(laszip_point* lzPoint, const size_t &globalIndex) const
{
    writeColorPacket(lzPoint,
                     globalIndex,
                     colors(),
                     scalars(CT_LasDefine::Red),
                     scalars(CT_LasDefine::Green),
                     scalars(CT_LasDefine::Blue));
}

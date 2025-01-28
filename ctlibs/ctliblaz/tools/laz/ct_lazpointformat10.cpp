#include "ct_lazpointformat10.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat10::CT_LAZPointFormat10(): CT_LAZPointFormat9()
{
}

size_t CT_LAZPointFormat10::sizeInBytes() const
{
    return CT_LAZPointFormat9::sizeInBytes() + colorPacketSizeInBytes() + nirPacketSizeInBytes();
}

void CT_LAZPointFormat10::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LAZPointFormat6::write(lzPoint, header, p, globalIndex);

    writeInfoFormat10(lzPoint, globalIndex);
    writeInfoFormat10(lzPoint, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat10::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LAZPointFormat9::typesToSearch();
    l.append(colorPacketTypesNeeded());
    l.append(nirPacketTypesNeeded());

    return l;
}

void CT_LAZPointFormat10::writeInfoFormat10(laszip_point* lzPoint, const size_t& globalIndex) const
{
    writeColorPacket(lzPoint,
                     globalIndex,
                     colors(),
                     scalars(CT_LasDefine::Red),
                     scalars(CT_LasDefine::Green),
                     scalars(CT_LasDefine::Blue));

    writeNirPacket(lzPoint, globalIndex, scalars(CT_LasDefine::NIR));
}

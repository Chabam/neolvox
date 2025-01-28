#include "ct_lazpointformat7.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat7::CT_LAZPointFormat7() : CT_LAZPointFormat6()
{
}

size_t CT_LAZPointFormat7::sizeInBytes() const
{
    return CT_LAZPointFormat6::sizeInBytes() + colorPacketSizeInBytes();
}

void CT_LAZPointFormat7::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LAZPointFormat6::write(lzPoint, header, p, globalIndex);

    writeInfoFormat7(lzPoint, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat7::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LAZPointFormat6::typesToSearch();
    l.append(colorPacketTypesNeeded());

    return l;
}

void CT_LAZPointFormat7::writeInfoFormat7(laszip_point* lzPoint, const size_t &globalIndex) const
{
    writeColorPacket(lzPoint,
                     globalIndex,
                     colors(),
                     scalars(CT_LasDefine::Red),
                     scalars(CT_LasDefine::Green),
                     scalars(CT_LasDefine::Blue));
}

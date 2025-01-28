#include "ct_laspointformat2.h"

CT_LASPointFormat2::CT_LASPointFormat2() : CT_LASPointFormat0()
{
}

size_t CT_LASPointFormat2::sizeInBytes() const
{
    return CT_LASPointFormat0::sizeInBytes() + colorPacketSizeInBytes();
}

void CT_LASPointFormat2::write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LASPointFormat0::write(stream, header, p, globalIndex);

    writeInfoFormat2(stream, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASPointFormat2::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LASPointFormat0::typesToSearch();
    l.append(colorPacketTypesNeeded());

    return l;
}

void CT_LASPointFormat2::writeInfoFormat2(QDataStream &stream, const size_t &globalIndex) const
{
    writeColorPacket(stream,
                     globalIndex,
                     colors(),
                     scalars(CT_LasDefine::Red),
                     scalars(CT_LasDefine::Green),
                     scalars(CT_LasDefine::Blue));
}

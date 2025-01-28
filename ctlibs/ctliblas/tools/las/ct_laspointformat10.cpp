#include "ct_laspointformat10.h"

CT_LASPointFormat10::CT_LASPointFormat10(): CT_LASPointFormat9()
{
}

size_t CT_LASPointFormat10::sizeInBytes() const
{
    return CT_LASPointFormat9::sizeInBytes() + colorPacketSizeInBytes() + nirPacketSizeInBytes();
}

void CT_LASPointFormat10::write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LASPointFormat6::write(stream, header, p, globalIndex);

    writeInfoFormat10(stream, globalIndex);
    writeInfoFormat9(stream, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASPointFormat10::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LASPointFormat9::typesToSearch();
    l.append(colorPacketTypesNeeded());
    l.append(nirPacketTypesNeeded());

    return l;
}

void CT_LASPointFormat10::writeInfoFormat10(QDataStream& stream, const size_t& globalIndex) const
{
    writeColorPacket(stream,
                     globalIndex,
                     colors(),
                     scalars(CT_LasDefine::Red),
                     scalars(CT_LasDefine::Green),
                     scalars(CT_LasDefine::Blue));

    writeNirPacket(stream, globalIndex, scalars(CT_LasDefine::NIR));
}

#include "ct_laspointformat9.h"

CT_LASPointFormat9::CT_LASPointFormat9() : CT_LASPointFormat6()
{
}

size_t CT_LASPointFormat9::sizeInBytes() const
{
    return CT_LASPointFormat6::sizeInBytes() + wavePacketSizeInBytes();
}

void CT_LASPointFormat9::write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LASPointFormat6::write(stream, header, p, globalIndex);

    writeInfoFormat9(stream, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASPointFormat9::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LASPointFormat6::typesToSearch();
    l.append(wavePacketTypesNeeded());

    return l;
}

void CT_LASPointFormat9::writeInfoFormat9(QDataStream &stream, const size_t& globalIndex) const
{
    writeWavePacket(stream,
                    globalIndex,
                    scalars(CT_LasDefine::Wave_Packet_Descriptor_Index),
                    scalars(CT_LasDefine::Byte_offset_to_waveform_data),
                    scalars(CT_LasDefine::Waveform_packet_size_in_bytes),
                    scalars(CT_LasDefine::Return_Point_Waveform_Location));
}

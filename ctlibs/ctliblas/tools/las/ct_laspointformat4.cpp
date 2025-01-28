#include "ct_laspointformat4.h"

CT_LASPointFormat4::CT_LASPointFormat4() : CT_LASPointFormat1()
{
}

size_t CT_LASPointFormat4::sizeInBytes() const
{
    return CT_LASPointFormat1::sizeInBytes() + wavePacketSizeInBytes();
}

void CT_LASPointFormat4::write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LASPointFormat1::write(stream, header, p, globalIndex);

    writeInfoFormat4(stream, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASPointFormat4::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LASPointFormat1::typesToSearch();
    l.append(wavePacketTypesNeeded());

    return l;
}

void CT_LASPointFormat4::writeInfoFormat4(QDataStream &stream, const size_t& globalIndex) const
{
    writeWavePacket(stream,
                    globalIndex,
                    scalars(CT_LasDefine::Wave_Packet_Descriptor_Index),
                    scalars(CT_LasDefine::Byte_offset_to_waveform_data),
                    scalars(CT_LasDefine::Waveform_packet_size_in_bytes),
                    scalars(CT_LasDefine::Return_Point_Waveform_Location));
}

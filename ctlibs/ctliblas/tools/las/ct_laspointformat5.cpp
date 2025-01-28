#include "ct_laspointformat5.h"

CT_LASPointFormat5::CT_LASPointFormat5() : CT_LASPointFormat3()
{
}

size_t CT_LASPointFormat5::sizeInBytes() const
{
    return CT_LASPointFormat3::sizeInBytes() + wavePacketSizeInBytes();
}

void CT_LASPointFormat5::write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LASPointFormat3::write(stream, header, p, globalIndex);

    writeInfoFormat5(stream, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASPointFormat5::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LASPointFormat3::typesToSearch();
    l.append(wavePacketTypesNeeded());

    return l;
}

void CT_LASPointFormat5::writeInfoFormat5(QDataStream &stream, const size_t& globalIndex) const
{
    writeWavePacket(stream,
                    globalIndex,
                    scalars(CT_LasDefine::Wave_Packet_Descriptor_Index),
                    scalars(CT_LasDefine::Byte_offset_to_waveform_data),
                    scalars(CT_LasDefine::Waveform_packet_size_in_bytes),
                    scalars(CT_LasDefine::Return_Point_Waveform_Location));
}

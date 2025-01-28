#include "ct_laswavepacket.h"

#include "abstract/ct_abstractlaspointformat.h"

CT_LASWavePacket::CT_LASWavePacket()
{
}

size_t CT_LASWavePacket::wavePacketSizeInBytes() const
{
    return sizeof(m_emptyData);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASWavePacket::wavePacketTypesNeeded()
{
    QList<CT_LasDefine::LASPointAttributesType> l;

    l.append(CT_LasDefine::Wave_Packet_Descriptor_Index);
    l.append(CT_LasDefine::Byte_offset_to_waveform_data);
    l.append(CT_LasDefine::Waveform_packet_size_in_bytes);
    l.append(CT_LasDefine::Return_Point_Waveform_Location);

    return l;
}

void CT_LASWavePacket::writeWavePacket(QDataStream &stream,
                                       const size_t& globalIndex,
                                       const CT_AbstractPointAttributesScalar* wpdiValues,
                                       const CT_AbstractPointAttributesScalar* bowdValues,
                                       const CT_AbstractPointAttributesScalar* wpsbValues,
                                       const CT_AbstractPointAttributesScalar* rpwlValues) const
{
    // wave packet descriptor index
    CT_WRITE_LAS_SCALAR_WITH(wpdiValues, quint8);

    // byte offset to waveform data
    CT_WRITE_LAS_SCALAR_WITH(bowdValues, quint64);

    // waveform packet size in bytes
    CT_WRITE_LAS_SCALAR_WITH(wpsbValues, quint32);

    // return point waveform location
    CT_WRITE_LAS_SCALAR_WITH(rpwlValues, float);

    // xt
    stream << float(0);

    // yt
    stream << float(0);

    // zt
    stream << float(0);
}

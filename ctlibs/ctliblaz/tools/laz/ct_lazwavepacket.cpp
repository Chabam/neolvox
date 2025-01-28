#include "ct_lazwavepacket.h"

#include "abstract/ct_abstractlazpointformat.h"

#include "laszip/laszip_api.h"

CT_LAZWavePacket::CT_LAZWavePacket()
{
}

size_t CT_LAZWavePacket::wavePacketSizeInBytes() const
{
    return sizeof(m_emptyData);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZWavePacket::wavePacketTypesNeeded()
{
    QList<CT_LasDefine::LASPointAttributesType> l;

    l.append(CT_LasDefine::Wave_Packet_Descriptor_Index);
    l.append(CT_LasDefine::Byte_offset_to_waveform_data);
    l.append(CT_LasDefine::Waveform_packet_size_in_bytes);
    l.append(CT_LasDefine::Return_Point_Waveform_Location);

    return l;
}

void CT_LAZWavePacket::writeWavePacket(laszip_point* lzPoint,
                                       const size_t& globalIndex,
                                       const CT_AbstractPointAttributesScalar* wpdiValues,
                                       const CT_AbstractPointAttributesScalar* bowdValues,
                                       const CT_AbstractPointAttributesScalar* wpsbValues,
                                       const CT_AbstractPointAttributesScalar* rpwlValues) const
{
    // wave packet descriptor index
    // CT_WRITE_LAS_SCALAR_WITH(wpdiValues, quint8);
    CT_WRITE_LAZ_WAVE_PACKET_ITEM_WITH(wpdiValues, quint8, LASZIP_POINT_WAVE_PACKET_WPDI);

    // byte offset to waveform data
    // CT_WRITE_LAS_SCALAR_WITH(bowdValues, quint64);
    CT_WRITE_LAZ_WAVE_PACKET_ITEM_WITH(bowdValues, quint64, LASZIP_POINT_WAVE_PACKET_BOWD);

    // waveform packet size in bytes
    // CT_WRITE_LAS_SCALAR_WITH(wpsbValues, quint32);
    CT_WRITE_LAZ_WAVE_PACKET_ITEM_WITH(wpsbValues, quint32, LASZIP_POINT_WAVE_PACKET_WPSB);

    // return point waveform location
    // CT_WRITE_LAS_SCALAR_WITH(rpwlValues, float);
    CT_WRITE_LAZ_WAVE_PACKET_ITEM_WITH(rpwlValues, float, LASZIP_POINT_WAVE_PACKET_RPWL);

    // xt
    // stream << float(0);
    *reinterpret_cast<float*>(lzPoint->wave_packet + LASZIP_POINT_WAVE_PACKET_XT) = float(0);

    // yt
    // stream << float(0);
    *reinterpret_cast<float*>(lzPoint->wave_packet + LASZIP_POINT_WAVE_PACKET_YT) = float(0);

    // zt
    // stream << float(0);
    *reinterpret_cast<float*>(lzPoint->wave_packet + LASZIP_POINT_WAVE_PACKET_ZT) = float(0);
}

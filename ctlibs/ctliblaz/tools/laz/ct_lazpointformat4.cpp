#include "ct_lazpointformat4.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat4::CT_LAZPointFormat4() : CT_LAZPointFormat1()
{
}

size_t CT_LAZPointFormat4::sizeInBytes() const
{
    return CT_LAZPointFormat1::sizeInBytes() + wavePacketSizeInBytes();
}

void CT_LAZPointFormat4::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LAZPointFormat1::write(lzPoint, header, p, globalIndex);

    writeInfoFormat4(lzPoint, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat4::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LAZPointFormat1::typesToSearch();
    l.append(wavePacketTypesNeeded());

    return l;
}

void CT_LAZPointFormat4::writeInfoFormat4(laszip_point* lzPoint, const size_t& globalIndex) const
{
    writeWavePacket(lzPoint,
                    globalIndex,
                    scalars(CT_LasDefine::Wave_Packet_Descriptor_Index),
                    scalars(CT_LasDefine::Byte_offset_to_waveform_data),
                    scalars(CT_LasDefine::Waveform_packet_size_in_bytes),
                    scalars(CT_LasDefine::Return_Point_Waveform_Location));
}

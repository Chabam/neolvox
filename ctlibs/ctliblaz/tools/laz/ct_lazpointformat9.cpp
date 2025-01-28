#include "ct_lazpointformat9.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat9::CT_LAZPointFormat9() : CT_LAZPointFormat6()
{
}

size_t CT_LAZPointFormat9::sizeInBytes() const
{
    return CT_LAZPointFormat6::sizeInBytes() + wavePacketSizeInBytes();
}

void CT_LAZPointFormat9::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    CT_LAZPointFormat6::write(lzPoint, header, p, globalIndex);

    writeInfoFormat9(lzPoint, globalIndex);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat9::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l = CT_LAZPointFormat6::typesToSearch();
    l.append(wavePacketTypesNeeded());

    return l;
}

void CT_LAZPointFormat9::writeInfoFormat9(laszip_point* lzPoint, const size_t& globalIndex) const
{
    writeWavePacket(lzPoint,
                    globalIndex,
                    scalars(CT_LasDefine::Wave_Packet_Descriptor_Index),
                    scalars(CT_LasDefine::Byte_offset_to_waveform_data),
                    scalars(CT_LasDefine::Waveform_packet_size_in_bytes),
                    scalars(CT_LasDefine::Return_Point_Waveform_Location));
}

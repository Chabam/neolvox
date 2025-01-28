#include "ct_laznirpacket.h"

#include "abstract/ct_abstractlazpointformat.h"

#include "laszip/laszip_api.h"

CT_LAZNirPacket::CT_LAZNirPacket()
{
}

size_t CT_LAZNirPacket::nirPacketSizeInBytes() const
{
    return sizeof(m_emptyData);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZNirPacket::nirPacketTypesNeeded()
{
    QList<CT_LasDefine::LASPointAttributesType> l;

    l.append(CT_LasDefine::NIR);

    return l;
}

void CT_LAZNirPacket::writeNirPacket(laszip_point* lzPoint,
                                     const size_t& globalIndex,
                                     const CT_AbstractPointAttributesScalar* nirValues) const
{
    CT_WRITE_LAZ_SCALAR_WITH(nirValues, quint16, rgb[3]) // nir
}

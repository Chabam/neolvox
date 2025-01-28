#include "ct_lasnirpacket.h"

#include "abstract/ct_abstractlaspointformat.h"

CT_LASNirPacket::CT_LASNirPacket()
{
}

size_t CT_LASNirPacket::nirPacketSizeInBytes() const
{
    return sizeof(m_emptyData);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASNirPacket::nirPacketTypesNeeded()
{
    QList<CT_LasDefine::LASPointAttributesType> l;

    l.append(CT_LasDefine::NIR);

    return l;
}

void CT_LASNirPacket::writeNirPacket(QDataStream &stream,
                                     const size_t& globalIndex,
                                     const CT_AbstractPointAttributesScalar* nirValues) const
{
    CT_WRITE_LAS_SCALAR_WITH(nirValues, quint16) // nir
}

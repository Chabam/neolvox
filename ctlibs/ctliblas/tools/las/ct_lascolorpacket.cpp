#include "ct_lascolorpacket.h"

#include "abstract/ct_abstractlaspointformat.h"

CT_LASColorPacket::CT_LASColorPacket()
{
}

size_t CT_LASColorPacket::colorPacketSizeInBytes() const
{
    return sizeof(m_emptyData);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASColorPacket::colorPacketTypesNeeded()
{
    QList<CT_LasDefine::LASPointAttributesType> l;

    l.append(CT_LasDefine::Red);
    l.append(CT_LasDefine::Green);
    l.append(CT_LasDefine::Blue);

    return l;
}

void CT_LASColorPacket::writeColorPacket(QDataStream &stream,
                                         const size_t& globalIndex,
                                         const CT_PointsAttributesColor* colorValues,
                                         const CT_AbstractPointAttributesScalar* redValues,
                                         const CT_AbstractPointAttributesScalar* greenValues,
                                         const CT_AbstractPointAttributesScalar* blueValues) const
{
    if((colorValues != nullptr) && colorValues->hasBeenSet(globalIndex))
    {
        // TODO : *257 ???
        const CT_Color& color = colorValues->constColorAt(globalIndex);
        stream << quint16(color.r()*257);
        stream << quint16(color.g()*257);
        stream << quint16(color.b()*257);
        return;
    }

    CT_WRITE_LAS_SCALAR_WITH(redValues, quint16);
    CT_WRITE_LAS_SCALAR_WITH(greenValues, quint16);
    CT_WRITE_LAS_SCALAR_WITH(blueValues, quint16);
}

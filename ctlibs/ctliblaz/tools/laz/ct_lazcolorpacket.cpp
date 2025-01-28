#include "ct_lazcolorpacket.h"

#include "abstract/ct_abstractlazpointformat.h"

#include "laszip/laszip_api.h"

CT_LAZColorPacket::CT_LAZColorPacket()
{
}

size_t CT_LAZColorPacket::colorPacketSizeInBytes() const
{
    return sizeof(m_emptyData);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZColorPacket::colorPacketTypesNeeded()
{
    QList<CT_LasDefine::LASPointAttributesType> l;

    l.append(CT_LasDefine::Red);
    l.append(CT_LasDefine::Green);
    l.append(CT_LasDefine::Blue);

    return l;
}

void CT_LAZColorPacket::writeColorPacket(laszip_point* lzPoint,
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
        lzPoint->rgb[0] = quint16(color.r()*257);
        lzPoint->rgb[1] = quint16(color.g()*257);
        lzPoint->rgb[2] = quint16(color.b()*257);
        return;
    }

    CT_WRITE_LAZ_SCALAR_WITH(redValues, quint16, rgb[0]);
    CT_WRITE_LAZ_SCALAR_WITH(greenValues, quint16, rgb[1]);
    CT_WRITE_LAZ_SCALAR_WITH(blueValues, quint16, rgb[2]);
}

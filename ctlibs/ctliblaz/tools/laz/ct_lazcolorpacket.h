#ifndef CT_LAZCOLORPACKET_H
#define CT_LAZCOLORPACKET_H

#include "ctliblaz/ctliblaz_global.h"

#include "ctliblas/tools/las/ct_lasdefine.h"
#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"
#include "ct_itemdrawable/ct_pointsattributescolor.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZColorPacket
{
public:
    CT_LAZColorPacket();

    /**
     * @brief Return the size of the color packet in bytes
     */
    size_t colorPacketSizeInBytes() const;

    /**
     * @brief Returns the types needed to write the color packet
     */
    QList<CT_LasDefine::LASPointAttributesType> colorPacketTypesNeeded();

    /**
     * @brief Write the information for the color packet
     */
    void writeColorPacket(laszip_point* lzPoint,
                          const size_t& globalIndex,
                          const CT_PointsAttributesColor* colorValues,
                          const CT_AbstractPointAttributesScalar* redValues,
                          const CT_AbstractPointAttributesScalar* greenValues,
                          const CT_AbstractPointAttributesScalar* blueValues) const;

private:
    char  m_emptyData[6];
};

#endif // CT_LAZCOLORPACKET_H

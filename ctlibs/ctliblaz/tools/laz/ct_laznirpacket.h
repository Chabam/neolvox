#ifndef CT_LAZNIRPACKET_H
#define CT_LAZNIRPACKET_H

#include "ctliblaz/ctliblaz_global.h"

#include "ctliblas/tools/las/ct_lasdefine.h"
#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZNirPacket
{
public:
    CT_LAZNirPacket();

    /**
     * @brief Return the size of the nir packet in bytes
     */
    size_t nirPacketSizeInBytes() const;

    /**
     * @brief Returns the types needed to write the nir packet
     */
    QList<CT_LasDefine::LASPointAttributesType> nirPacketTypesNeeded();

    /**
     * @brief Write the information for the nir packet
     */
    void writeNirPacket(laszip_point* lzPoint, const size_t& globalIndex, const CT_AbstractPointAttributesScalar* nirValues) const;

private:
    char  m_emptyData[2];
};

#endif // CT_LAZNIRPACKET_H

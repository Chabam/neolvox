#ifndef CT_LAZGPSPACKET_H
#define CT_LAZGPSPACKET_H

#include "ctliblaz/ctliblaz_global.h"

#include "ctliblas/tools/las/ct_lasdefine.h"
#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZGpsPacket
{
public:
    CT_LAZGpsPacket();

    /**
     * @brief Return the size of the gps packet in bytes
     */
    size_t gpsPacketSizeInBytes() const;

    /**
     * @brief Returns the types needed to write the gps packet
     */
    QList<CT_LasDefine::LASPointAttributesType> gpsPacketTypesNeeded();

    /**
     * @brief Write the information for the gps packet
     */
    void writeGpsPacket(laszip_point* lzPoint, const size_t& globalIndex, const CT_AbstractPointAttributesScalar* gpsValues) const;

private:
    char  m_emptyData[8];
};

#endif // CT_LAZGPSPACKET_H

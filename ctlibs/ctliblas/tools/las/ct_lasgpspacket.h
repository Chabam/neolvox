#ifndef CT_LASGPSPACKET_H
#define CT_LASGPSPACKET_H

#include "ctliblas/tools/las/ct_lasdefine.h"
#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"

#include <QDataStream>

class CTLIBLAS_EXPORT CT_LASGpsPacket
{
public:
    CT_LASGpsPacket();

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
    void writeGpsPacket(QDataStream &stream, const size_t& globalIndex, const CT_AbstractPointAttributesScalar* gpsValues) const;

private:
    char  m_emptyData[8];
};

#endif // CT_LASGPSPACKET_H

#ifndef CT_LASNIRPACKET_H
#define CT_LASNIRPACKET_H

#include "ctliblas/tools/las/ct_lasdefine.h"
#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"

#include <QDataStream>

class CTLIBLAS_EXPORT CT_LASNirPacket
{
public:
    CT_LASNirPacket();

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
    void writeNirPacket(QDataStream &stream, const size_t& globalIndex, const CT_AbstractPointAttributesScalar* nirValues) const;

private:
    char  m_emptyData[2];
};

#endif // CT_LASNIRPACKET_H

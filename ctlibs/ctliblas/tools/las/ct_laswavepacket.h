#ifndef CT_LASWAVEPACKET_H
#define CT_LASWAVEPACKET_H

#include "ctliblas/tools/las/ct_lasdefine.h"
#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"

#include <QDataStream>

class CTLIBLAS_EXPORT CT_LASWavePacket
{
public:
    CT_LASWavePacket();

    /**
     * @brief Return the size of the wave packet in bytes
     */
    size_t wavePacketSizeInBytes() const;

    /**
     * @brief Returns the types needed to write the wave packet
     */
    QList<CT_LasDefine::LASPointAttributesType> wavePacketTypesNeeded();

    /**
     * @brief Write the information for the wave packet
     */
    void writeWavePacket(QDataStream &stream,
                         const size_t& globalIndex,
                         const CT_AbstractPointAttributesScalar* wpdiValues,
                         const CT_AbstractPointAttributesScalar* bowdValues,
                         const CT_AbstractPointAttributesScalar* wpsbValues,
                         const CT_AbstractPointAttributesScalar* rpwlValues) const;

private:
    char  m_emptyData[29];
};

#endif // CT_LASWAVEPACKET_H

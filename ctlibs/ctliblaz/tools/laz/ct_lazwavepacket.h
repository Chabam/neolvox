#ifndef CT_LAZWAVEPACKET_H
#define CT_LAZWAVEPACKET_H

#include "ctliblaz/ctliblaz_global.h"

#include "ctliblas/tools/las/ct_lasdefine.h"
#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZWavePacket
{
public:
    CT_LAZWavePacket();

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
    void writeWavePacket(laszip_point* lzPoint,
                         const size_t& globalIndex,
                         const CT_AbstractPointAttributesScalar* wpdiValues,
                         const CT_AbstractPointAttributesScalar* bowdValues,
                         const CT_AbstractPointAttributesScalar* wpsbValues,
                         const CT_AbstractPointAttributesScalar* rpwlValues) const;

private:
    char  m_emptyData[29];
};

#endif // CT_LAZWAVEPACKET_H

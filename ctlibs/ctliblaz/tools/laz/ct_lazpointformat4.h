#ifndef CT_LAZPOINTFORMAT4_H
#define CT_LAZPOINTFORMAT4_H

#include "ctliblaz/tools/laz/ct_lazpointformat1.h"
#include "ctliblaz/tools/laz/ct_lazwavepacket.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZPointFormat4 : public CT_LAZPointFormat1, public CT_LAZWavePacket
{
public:
    CT_LAZPointFormat4();

    /**
     * @brief Return the size of the points in byte
     */
    size_t sizeInBytes() const override;

    /**
     * @brief Write the point 'globalIndex' (index in the global pointCloud)
     */
    void write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const override;

protected:
    /**
     * @brief Derivated class must return attribute types that they need to write a point
     */
    QList<CT_LasDefine::LASPointAttributesType> typesToSearch() override;

    /**
     * @brief Write only the information of the point format 4
     */
    void writeInfoFormat4(laszip_point* lzPoint, const size_t& globalIndex) const;
};

#endif // CT_LAZPOINTFORMAT4_H

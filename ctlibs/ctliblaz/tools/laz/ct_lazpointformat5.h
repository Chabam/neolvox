#ifndef CT_LAZPOINTFORMAT5_H
#define CT_LAZPOINTFORMAT5_H

#include "ctliblaz/tools/laz/ct_lazpointformat3.h"
#include "ctliblaz/tools/laz/ct_lazwavepacket.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZPointFormat5 : public CT_LAZPointFormat3, public CT_LAZWavePacket
{
public:
    CT_LAZPointFormat5();

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
     * @brief Write only the information of the point format 5
     */
    void writeInfoFormat5(laszip_point* lzPoint, const size_t& globalIndex) const;
};

#endif // CT_LAZPOINTFORMAT5_H

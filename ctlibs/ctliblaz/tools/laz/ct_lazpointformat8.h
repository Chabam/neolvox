#ifndef CT_LAZPOINTFORMAT8_H
#define CT_LAZPOINTFORMAT8_H

#include "ctliblaz/tools/laz/ct_lazpointformat7.h"
#include "ctliblaz/tools/laz/ct_laznirpacket.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZPointFormat8 : public CT_LAZPointFormat7, public CT_LAZNirPacket
{
public:
    CT_LAZPointFormat8();

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
    virtual QList<CT_LasDefine::LASPointAttributesType> typesToSearch() override;

    /**
     * @brief Write only the information of the point format 8
     */
    void writeInfoFormat8(laszip_point* lzPoint, const size_t& globalIndex) const;
};

#endif // CT_LAZPOINTFORMAT8_H

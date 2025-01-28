#ifndef CT_LAZPOINTFORMAT1_H
#define CT_LAZPOINTFORMAT1_H

#include "ctliblaz/tools/laz/ct_lazpointformat0.h"
#include "ctliblaz/tools/laz/ct_lazgpspacket.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZPointFormat1 : public CT_LAZPointFormat0, public CT_LAZGpsPacket
{
public:
    CT_LAZPointFormat1();

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
     * @brief Write only the information of the point format 1
     */
    void writeInfoFormat1(laszip_point* lzPoint, const size_t& globalIndex) const;
};

#endif // CT_LAZPOINTFORMAT1_H

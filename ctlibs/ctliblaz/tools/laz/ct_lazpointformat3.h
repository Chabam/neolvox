#ifndef CT_LAZPOINTFORMAT3_H
#define CT_LAZPOINTFORMAT3_H

#include "ctliblaz/tools/laz/ct_lazpointformat2.h"
#include "ctliblaz/tools/laz/ct_lazgpspacket.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZPointFormat3 : public CT_LAZPointFormat2, public CT_LAZGpsPacket
{
public:
    CT_LAZPointFormat3();

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
     * @brief Write only the information of the point format 3
     */
    void writeInfoFormat3(laszip_point* lzPoint, const size_t& globalIndex) const;
};

#endif // CT_LAZPOINTFORMAT3_H

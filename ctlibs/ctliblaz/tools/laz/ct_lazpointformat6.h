#ifndef CT_LAZPOINTFORMAT6_H
#define CT_LAZPOINTFORMAT6_H

#include "abstract/ct_abstractlazpointformat.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZPointFormat6 : public CT_AbstractLAZPointFormat
{
public:
    CT_LAZPointFormat6();

    /**
     * @brief Return the size of the points in byte
     */
    size_t sizeInBytes() const override;

    /**
     * @brief Write the point 'globalIndex' (index in the global pointCloud)
     */
    void write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const override;

private:
    char                                    m_emptyData[30];

protected:
    /**
     * @brief Derivated class must return attribute types that they need to write a point
     */
    virtual QList<CT_LasDefine::LASPointAttributesType> typesToSearch() override;
};

#endif // CT_LAZPOINTFORMAT6_H

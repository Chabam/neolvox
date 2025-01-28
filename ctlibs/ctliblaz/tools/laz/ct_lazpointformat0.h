#ifndef CT_LAZPOINTFORMAT0_H
#define CT_LAZPOINTFORMAT0_H

#include "ctliblas/ctliblas_global.h"
#include "ctliblaz/ctliblaz_global.h"

#include "ctliblaz/tools/laz/abstract/ct_abstractlazpointformat.h"

#include "laszip/laszip_api.h"

/**
 * @brief A tool class that can be use to write a point of format 0 in a LAZ File
 */
class CTLIBLAZ_EXPORT CT_LAZPointFormat0 : public CT_AbstractLAZPointFormat
{
public:
    CT_LAZPointFormat0();

    /**
     * @brief Return the size of the points in byte
     */
    size_t sizeInBytes() const override;

    /**
     * @brief Write the point 'globalIndex' (index in the global pointCloud)
     */
    void write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const override;

private:
    char                                    m_emptyData[20];

protected:
    /**
     * @brief Derivated class must return attribute types that they need to write a point
     */
    QList<CT_LasDefine::LASPointAttributesType> typesToSearch() override;
};

#endif // CT_LAZPOINTFORMAT0_H

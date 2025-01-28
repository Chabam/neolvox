#ifndef CT_LASPOINTFORMAT0_H
#define CT_LASPOINTFORMAT0_H

#include "ctliblas/ctliblas_global.h"

#include "ctliblas/tools/las/abstract/ct_abstractlaspointformat.h"

/**
 * @brief A tool class that can be use to write a point of format 0 in a LAS File
 */
class CTLIBLAS_EXPORT CT_LASPointFormat0 : public CT_AbstractLASPointFormat
{
public:
    CT_LASPointFormat0();

    /**
     * @brief Return the size of the points in byte
     */
    size_t sizeInBytes() const override;

    /**
     * @brief Write the point 'globalIndex' (index in the global pointCloud)
     */
    void write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const override;

private:
    char                                    m_emptyData[20];

protected:
    /**
     * @brief Derivated class must return attribute types that they need to write a point
     */
    QList<CT_LasDefine::LASPointAttributesType> typesToSearch() override;
};

#endif // CT_LASPOINTFORMAT0_H

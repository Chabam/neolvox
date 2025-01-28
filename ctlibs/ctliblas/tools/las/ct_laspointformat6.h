#ifndef CT_LASPOINTFORMAT6_H
#define CT_LASPOINTFORMAT6_H

#include "abstract/ct_abstractlaspointformat.h"

class CTLIBLAS_EXPORT CT_LASPointFormat6 : public CT_AbstractLASPointFormat
{
public:
    CT_LASPointFormat6();

    /**
     * @brief Return the size of the points in byte
     */
    size_t sizeInBytes() const override;

    /**
     * @brief Write the point 'globalIndex' (index in the global pointCloud)
     */
    void write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const override;

private:
    char                                    m_emptyData[30];

protected:
    /**
     * @brief Derivated class must return attribute types that they need to write a point
     */
    virtual QList<CT_LasDefine::LASPointAttributesType> typesToSearch() override;
};

#endif // CT_LASPOINTFORMAT6_H

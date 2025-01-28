#ifndef CT_LASPOINTFORMAT3_H
#define CT_LASPOINTFORMAT3_H

#include "ctliblas/tools/las/ct_laspointformat2.h"
#include "ctliblas/tools/las/ct_lasgpspacket.h"

class CTLIBLAS_EXPORT CT_LASPointFormat3 : public CT_LASPointFormat2, public CT_LASGpsPacket
{
public:
    CT_LASPointFormat3();

    /**
     * @brief Return the size of the points in byte
     */
    size_t sizeInBytes() const override;

    /**
     * @brief Write the point 'globalIndex' (index in the global pointCloud)
     */
    void write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const override;

protected:
    /**
     * @brief Derivated class must return attribute types that they need to write a point
     */
    virtual QList<CT_LasDefine::LASPointAttributesType> typesToSearch() override;

    /**
     * @brief Write only the information of the point format 3
     */
    void writeInfoFormat3(QDataStream &stream, const size_t& globalIndex) const;
};

#endif // CT_LASPOINTFORMAT3_H

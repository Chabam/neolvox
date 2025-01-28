#ifndef CT_LASPOINTFORMAT10_H
#define CT_LASPOINTFORMAT10_H

#include "ctliblas/tools/las/ct_laspointformat9.h"
#include "ctliblas/tools/las/ct_lascolorpacket.h"
#include "ctliblas/tools/las/ct_lasnirpacket.h"

class CTLIBLAS_EXPORT CT_LASPointFormat10 : public CT_LASPointFormat9, public CT_LASColorPacket, public CT_LASNirPacket
{
public:
    CT_LASPointFormat10();

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
    QList<CT_LasDefine::LASPointAttributesType> typesToSearch() override;

    /**
     * @brief Write only the information of the point format 10
     */
    void writeInfoFormat10(QDataStream &stream, const size_t &globalIndex) const;
};

#endif // CT_LASPOINTFORMAT10_H

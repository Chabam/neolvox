#ifndef CT_LASPOINTFORMAT9_H
#define CT_LASPOINTFORMAT9_H

#include "ctliblas/tools/las/ct_laspointformat6.h"
#include "ctliblas/tools/las/ct_laswavepacket.h"

class CTLIBLAS_EXPORT CT_LASPointFormat9 : public CT_LASPointFormat6, public CT_LASWavePacket
{
public:
    CT_LASPointFormat9();

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
     * @brief Write only the information of the point format 9
     */
    void writeInfoFormat9(QDataStream &stream, const size_t &globalIndex) const;
};

#endif // CT_LASPOINTFORMAT9_H

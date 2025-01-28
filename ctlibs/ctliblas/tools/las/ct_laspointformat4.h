#ifndef CT_LASPOINTFORMAT4_H
#define CT_LASPOINTFORMAT4_H

#include "ctliblas/tools/las/ct_laspointformat1.h"
#include "ctliblas/tools/las/ct_laswavepacket.h"

class CTLIBLAS_EXPORT CT_LASPointFormat4 : public CT_LASPointFormat1, public CT_LASWavePacket
{
public:
    CT_LASPointFormat4();

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
     * @brief Write only the information of the point format 4
     */
    void writeInfoFormat4(QDataStream &stream, const size_t& globalIndex) const;
};

#endif // CT_LASPOINTFORMAT4_H

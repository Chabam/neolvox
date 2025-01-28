#ifndef CT_ABSTRACTFILTER_LAS_H
#define CT_ABSTRACTFILTER_LAS_H

#include "ctliblas/ctliblas_global.h"
#include "ctliblas/itemdrawable/las/ct_stdlaspointsattributescontainer.h"
#include "ct_filter/abstract/ct_abstractfilter_xyz.h"

class CTLIBLAS_EXPORT CT_AbstractFilter_LAS : public CT_AbstractFilter_XYZ
{
    Q_OBJECT

public:
    CT_AbstractFilter_LAS(QString pluginName);
    CT_AbstractFilter_LAS(const CT_AbstractFilter_LAS& other);

    /**
     * @brief Set LAS attributes to use.
     * @return false if it was nullptr
     */
    virtual bool setLASAttributesContainer(const CT_StdLASPointsAttributesContainer *lasAttributes);

    /**
     * @brief Inherited to use las data
     */
    bool filterPointCloudIndex() override;

    /**
     * @brief Call to check if a point is valid (not filtered) or not (filtered)
     * @param pointIt : the iterator that was on the current point to check
     * @param LASData : the las data for the curren point
     * @return true if the point must be kept (not filtered)
     */
    virtual bool validatePoint(const CT_PointIterator &pointIt, const CT_LASData &LASData) = 0;

    /**
     * @brief Inherited to use las data if exist
     */
    bool validatePoint(const CT_PointIterator &pointIt) override;

private:
    CT_StdLASPointsAttributesContainer  *_lasAttributes;

protected:
    /**
     * @brief Returns las attributes
     */
    CT_StdLASPointsAttributesContainer* lasAttributes() const;
};

#endif // CT_ABSTRACTFILTER_LAS_H

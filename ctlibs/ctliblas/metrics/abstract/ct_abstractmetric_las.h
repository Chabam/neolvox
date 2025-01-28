#ifndef CT_ABSTRACTMETRIC_LAS_H
#define CT_ABSTRACTMETRIC_LAS_H

#include "ctliblas/ctliblas_global.h"
#include "ctliblas/itemdrawable/las/ct_stdlaspointsattributescontainer.h"

#include "ct_metric/abstract/ct_abstractmetric_xyz.h"

class CTLIBLAS_EXPORT CT_AbstractMetric_LAS : public CT_AbstractMetric_XYZ
{
    Q_OBJECT

public:
    CT_AbstractMetric_LAS(QString pluginName);
    CT_AbstractMetric_LAS(const CT_AbstractMetric_LAS& other);

    bool initLasDatas(const CT_AbstractPointCloudIndex* inCloud, const CT_AreaShape2DData *plotArea, const CT_StdLASPointsAttributesContainer *lasAttributes);

private:
    CT_StdLASPointsAttributesContainer  *_lasAttributes;

protected:
    /**
     * @brief Returns las attributes
     */
    CT_StdLASPointsAttributesContainer* lasAttributes() const;
};

#endif // CT_ABSTRACTMETRIC_LAS_H

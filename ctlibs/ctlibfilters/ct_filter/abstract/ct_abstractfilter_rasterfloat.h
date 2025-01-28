#ifndef CT_ABSTRACTFILTER_RASTERFLOAT_H
#define CT_ABSTRACTFILTER_RASTERFLOAT_H

#include "ctlibfilters_global.h"

#include "ct_filter/abstract/ct_abstractfilter.h"
#include "ct_itemdrawable/ct_image2d.h"

/**
 * @brief A rasterFloat filter create an output float raster based on an input float raster
 */
class CTLIBFILTERS_EXPORT CT_AbstractFilter_RasterFloat : public CT_AbstractFilter
{
    Q_OBJECT
public:
    CT_AbstractFilter_RasterFloat(QString pluginName);
    CT_AbstractFilter_RasterFloat(const CT_AbstractFilter_RasterFloat &other);

    ~CT_AbstractFilter_RasterFloat();

    /**
     * @brief Set the input raster
     * @param inItem : a raster
     * @return true if not null
     */
    bool setInputRaster(const CT_Image2D<float>* inItem);

    /**
     * @brief Call to filter input raster.
     * @return false if the input rester was not set or another error is occured
     */
    virtual bool filterRaster();

    /**
     * @brief Return the output cloud generated in method "filterPointCloudIndex".
     */
    const CT_Image2D<float> *outputRaster() const;

    /**
     * @brief Take the output raster generated in method "filterPointCloudIndex". You will be the owner of the raster and must manage is life in memory.
     */
    CT_Image2D<float>* takeOutputRaster();

    virtual void finalizeConfiguration() override {}


protected:

    const CT_Image2D<float>*            _inRaster;
    CT_Image2D<float>*                  _outRaster;

    /**
     * @brief Redefine to implement filter
     * @return false if an error is occured
     */
    virtual bool computeOutputRaster();

    /**
     * @brief Return the input cloud
     */
    const CT_Image2D<float>* inputRaster() const;
};

#endif // CT_ABSTRACTFILTER_RASTERFLOAT_H

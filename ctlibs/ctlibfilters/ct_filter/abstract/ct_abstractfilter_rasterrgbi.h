#ifndef CT_ABSTRACTFILTER_RASTERRGBI_H
#define CT_ABSTRACTFILTER_RASTERRGBI_H

#include "ctlibfilters_global.h"

#include "ct_filter/abstract/ct_abstractfilter.h"
#include "ct_itemdrawable/ct_image2d.h"

/**
 * @brief A rasterFloat filter create an output float raster based on some input float raster R V B RE PIR MIR
 */
class CTLIBFILTERS_EXPORT CT_AbstractFilter_RasterRGBI : public CT_AbstractFilter
{
    Q_OBJECT
public:
    CT_AbstractFilter_RasterRGBI(QString pluginName);
    CT_AbstractFilter_RasterRGBI(const CT_AbstractFilter_RasterRGBI &other);

    ~CT_AbstractFilter_RasterRGBI();

    /**
     * @brief Set the input raster
     * @param inR : a raster
     * @param inG : a raster
     * @param inB : a raster
     * @param inRE : a raster
     * @param inNIR : a raster
     * @param inMIR : a raster
     * @return true if not null
     */
    bool setInputRaster(const CT_Image2D<float>* inR,
                        const CT_Image2D<float>* inG,
                        const CT_Image2D<float>* inB,
                        const CT_Image2D<float>* inRE,
                        const CT_Image2D<float>* inNIR,
                        const CT_Image2D<float>* inMIR);

    /**
     * @brief Call to filter input raster.
     * @return false if the input rester was not set or another error is occured
     */
    virtual bool filterRaster();

    /**
     * @brief Return the output raster generated in method "filterRaster".
     */
    const CT_Image2D<float> *outputRaster() const;

    /**
     * @brief Take the output raster generated in method "filterRaster". You will be the owner of the raster and must manage is life in memory.
     */
    CT_Image2D<float>* takeOutputRaster();

    virtual void finalizeConfiguration() override {}


protected:

    const CT_Image2D<float>*            _inRasterR; // Red
    const CT_Image2D<float>*            _inRasterG; // Green
    const CT_Image2D<float>*            _inRasterB; // Blue
    const CT_Image2D<float>*            _inRasterRE; // Red Edge
    const CT_Image2D<float>*            _inRasterNIR; // Near Infrared
    const CT_Image2D<float>*            _inRasterMIR; //Mid infrared

    CT_Image2D<float>*                  _outRaster;

    /**
     * @brief Redefine to implement filter
     * @return false if an error is occured
     */
    virtual bool computeOutputRaster();

    /**
     * @brief Return the input Red raster
     */
    const CT_Image2D<float>* inputRasterR() const;
    /**
     * @brief Return the input Red raster
     */
    const CT_Image2D<float>* inputRasterG() const;
    /**
     * @brief Return the input Red raster
     */
    const CT_Image2D<float>* inputRasterB() const;
    /**
     * @brief Return the input Red raster
     */
    const CT_Image2D<float>* inputRasterRE() const;
    /**
     * @brief Return the input Red raster
     */
    const CT_Image2D<float>* inputRasterNIR() const;
    /**
     * @brief Return the input Red raster
     */
    const CT_Image2D<float>* inputRasterMIR() const;
};

#endif // CT_ABSTRACTFILTER_RASTERRGBI_H

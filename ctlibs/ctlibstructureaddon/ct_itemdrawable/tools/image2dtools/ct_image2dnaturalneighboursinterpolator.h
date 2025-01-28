#ifndef CT_IMAGE2DNATURALNEIGHBOURSINTERPOLATOR_H
#define CT_IMAGE2DNATURALNEIGHBOURSINTERPOLATOR_H

#include "ctlibstructure_global.h"
#include "ct_itemdrawable/ct_image2d.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_Image2DNaturalNeighboursInterpolator
{
public:
    CT_Image2DNaturalNeighboursInterpolator(const CT_Image2D<float> *raster,
                                            CT_Image2D<float>* rasterI,
                                            int ncells);

    typedef void result_type;
    void operator()(const size_t& index)
    {
        interpolateForOneCell(index);
    }

    void interpolateForOneCell(const size_t& index);

private:
    const CT_Image2D<float>*  _raster;
    CT_Image2D<float>*        _rasterInterpol;
    int                      _nCells;
};

#endif // CT_IMAGE2DNATURALNEIGHBOURSINTERPOLATOR_H

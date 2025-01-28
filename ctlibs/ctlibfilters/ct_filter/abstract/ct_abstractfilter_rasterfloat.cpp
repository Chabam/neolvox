#include "ct_abstractfilter_rasterfloat.h"


CT_AbstractFilter_RasterFloat::CT_AbstractFilter_RasterFloat(QString pluginName) : CT_AbstractFilter(pluginName)
{
    _inRaster = nullptr;
    _outRaster = nullptr;
}

CT_AbstractFilter_RasterFloat::CT_AbstractFilter_RasterFloat(const CT_AbstractFilter_RasterFloat &other) : CT_AbstractFilter(other)
{
    _inRaster = other._inRaster;
    _outRaster = nullptr;

    if (other._outRaster != nullptr)
    {
        _outRaster = new CT_Image2D<float>(_inRaster->minX(), _inRaster->minY(), _inRaster->xdim(), _inRaster->ydim(), _inRaster->resolution(), _inRaster->level(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        for (size_t c = 0 ; c < _inRaster->nCells() ; c++)
        {
            _outRaster->setValueAtIndex(c, other._outRaster->valueAtIndex(c));
        }
    }
}

CT_AbstractFilter_RasterFloat::~CT_AbstractFilter_RasterFloat()
{
    delete _outRaster;
}

bool CT_AbstractFilter_RasterFloat::setInputRaster(const CT_Image2D<float>* inItem)

{
    if (inItem == nullptr) {return false;}
    _inRaster = inItem;

    return true;
}


bool CT_AbstractFilter_RasterFloat::filterRaster()
{
    if (_inRaster == nullptr) {return false;}
    delete _outRaster;

    _outRaster = new CT_Image2D<float>(_inRaster->minX(), _inRaster->minY(), _inRaster->xdim(), _inRaster->ydim(), _inRaster->resolution(), _inRaster->level(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

    return computeOutputRaster();
}

// by default return a copy of input raster
bool CT_AbstractFilter_RasterFloat::computeOutputRaster()
{
    for (size_t c = 0 ; c < _inRaster->nCells() ; c++)
    {
        _outRaster->setValueAtIndex(c, _inRaster->valueAtIndex(c));
    }

    _outRaster->computeMinMax();
    return true;
}

CT_Image2D<float> *CT_AbstractFilter_RasterFloat::takeOutputRaster()
{
    CT_Image2D<float>* out = _outRaster;
    _outRaster = nullptr;

    return out;
}

const CT_Image2D<float> *CT_AbstractFilter_RasterFloat::inputRaster() const
{
    return _inRaster;
}

const CT_Image2D<float> *CT_AbstractFilter_RasterFloat::outputRaster() const
{
    return _outRaster;
}


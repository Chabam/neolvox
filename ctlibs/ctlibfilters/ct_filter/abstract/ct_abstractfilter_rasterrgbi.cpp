#include "ct_abstractfilter_rasterrgbi.h"


CT_AbstractFilter_RasterRGBI::CT_AbstractFilter_RasterRGBI(QString pluginName) : CT_AbstractFilter(pluginName)
{
    _inRasterR = nullptr;
    _inRasterG = nullptr;
    _inRasterB = nullptr;
    _inRasterRE = nullptr;
    _inRasterNIR = nullptr;
    _inRasterMIR = nullptr;
    _outRaster = nullptr;
}

CT_AbstractFilter_RasterRGBI::CT_AbstractFilter_RasterRGBI(const CT_AbstractFilter_RasterRGBI &other) : CT_AbstractFilter(other)
{
    _inRasterR = other._inRasterR;
    _inRasterG = other._inRasterG;
    _inRasterB = other._inRasterB;
    _inRasterRE = other._inRasterRE;
    _inRasterNIR = other._inRasterNIR;
    _inRasterMIR = other._inRasterMIR;
    _outRaster = nullptr;

    if (other._outRaster != nullptr)
    {
        _outRaster = new CT_Image2D<float>(other._outRaster->minX(), other._outRaster->minY(), other._outRaster->xdim(), other._outRaster->ydim(), other._outRaster->resolution(), other._outRaster->level(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        for (size_t c = 0 ; c < other._outRaster->nCells() ; c++)
        {
            _outRaster->setValueAtIndex(c, other._outRaster->valueAtIndex(c));
        }
    }
}

CT_AbstractFilter_RasterRGBI::~CT_AbstractFilter_RasterRGBI()
{
    delete _outRaster;
}

bool CT_AbstractFilter_RasterRGBI::setInputRaster(const CT_Image2D<float>* inR,
                                                  const CT_Image2D<float>* inG,
                                                  const CT_Image2D<float>* inB,
                                                  const CT_Image2D<float>* inRE,
                                                  const CT_Image2D<float>* inNIR,
                                                  const CT_Image2D<float>* inMIR)

{
    if (inR == nullptr) {return false;}
    if (inG == nullptr) {return false;}
    if (inB == nullptr) {return false;}
    if (inNIR == nullptr) {return false;}

    _inRasterR = inR;
    _inRasterG = inG;
    _inRasterB = inB;
    _inRasterRE = inRE;
    _inRasterNIR = inNIR;
    _inRasterMIR = inMIR;

    return true;
}


bool CT_AbstractFilter_RasterRGBI::filterRaster()
{
    if (_inRasterR == nullptr) {return false;}
    if (_inRasterG == nullptr) {return false;}
    if (_inRasterB == nullptr) {return false;}
    if (_inRasterNIR == nullptr) {return false;}

    // TO DO : improve tests
    if (!(_inRasterR->compare(_inRasterG))) {return false;}
    if (!(_inRasterR->compare(_inRasterB))) {return false;}
    if (!(_inRasterR->compare(_inRasterNIR))) {return false;}
    if (_inRasterRE != nullptr && !_inRasterR->compare(_inRasterRE)) {return false;}
    if (_inRasterMIR != nullptr && !_inRasterR->compare(_inRasterMIR)) {return false;}

    delete _outRaster;
    _outRaster = new CT_Image2D<float>(_inRasterR->minX(), _inRasterR->minY(), _inRasterR->xdim(), _inRasterR->ydim(), _inRasterR->resolution(), _inRasterR->level(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

    return computeOutputRaster();
}

// by default return a copy of input raster
bool CT_AbstractFilter_RasterRGBI::computeOutputRaster()
{
    for (size_t c = 0 ; c < _inRasterR->nCells() ; c++)
    {
        _outRaster->setValueAtIndex(c, _inRasterR->valueAtIndex(c));
    }

    _outRaster->computeMinMax();
    return true;
}

CT_Image2D<float> *CT_AbstractFilter_RasterRGBI::takeOutputRaster()
{
    CT_Image2D<float>* out = _outRaster;
    _outRaster = nullptr;

    return out;
}

const CT_Image2D<float> *CT_AbstractFilter_RasterRGBI::inputRasterR() const
{
    return _inRasterR;
}

const CT_Image2D<float> *CT_AbstractFilter_RasterRGBI::inputRasterG() const
{
    return _inRasterG;
}

const CT_Image2D<float> *CT_AbstractFilter_RasterRGBI::inputRasterB() const
{
    return _inRasterB;
}

const CT_Image2D<float> *CT_AbstractFilter_RasterRGBI::inputRasterRE() const
{
    return _inRasterRE;
}

const CT_Image2D<float> *CT_AbstractFilter_RasterRGBI::inputRasterNIR() const
{
    return _inRasterNIR;
}

const CT_Image2D<float> *CT_AbstractFilter_RasterRGBI::inputRasterMIR() const
{
    return _inRasterMIR;
}

const CT_Image2D<float> *CT_AbstractFilter_RasterRGBI::outputRaster() const
{
    return _outRaster;
}


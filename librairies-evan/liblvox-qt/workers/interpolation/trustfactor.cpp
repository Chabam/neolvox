#include "trustfactor.h"

TrustFactor::TrustFactor(const Grid3Df* originalDensityGrid, const Grid3Di* beforeGrid, const Grid3Di* theoriticalsGrid,
                         Grid3Df* outDensityGrid, double radius, int32_t effectiveRayThreshold, int32_t endRayThreshold, lvox::Logger* logger)
    : WorkerBase(logger), _originalDensityGrid(originalDensityGrid), _beforeGrid(beforeGrid), _theoriticalsGrid(theoriticalsGrid),
      _outDensityGrid(outDensityGrid)
{
    _radius = radius;
    _effectiveRayThreshold = effectiveRayThreshold;
    _endRayThreshold = endRayThreshold;
}

void TrustFactor::run()
{
    TrustInterpolationVisitor visitor(*_originalDensityGrid, *_outDensityGrid, *_beforeGrid, *_theoriticalsGrid,
                                      _effectiveRayThreshold, _endRayThreshold);

    std::vector<PropagationVisitor*> l;
    l.push_back(&visitor);

    Grid3DPropagationAlgorithm algo(*_originalDensityGrid, l, _radius);

    const size_t nCells = _originalDensityGrid->cellCount();

    for (size_t i = 0; i < nCells; ++i)
    {
        const float density = _originalDensityGrid->getValueAtIndex(i);

        if (density == static_cast<float>(lvox::Grid3d<float>::ErrorOrWarningCode::Nt_Minus_Nb_Inferior_Threshold))
        {
            algo.startFromCell(i);
        }
    }
}

#ifndef TRUSTFACTOR_H
#define TRUSTFACTOR_H

#pragma once

#include "../../datastructures/types.h"
#include "../workerbase.h"

#include "../../traversal/propagation/grid3dpropagationalgorithm.h"
#include "../../traversal/propagation/visitor/trustinterpolationvisitor.h"

class TrustFactor : public WorkerBase
{
public:
    TrustFactor(const Grid3Df* originalDensityGrid, const Grid3Di* beforeGrid, const Grid3Di* theoriticalsGrid,
                Grid3Df* outDensityGrid, double radius, int32_t effectiveRayThreshold, int32_t endRayThreshold, lvox::Logger* logger);

    void run() override;

private:
    const Grid3Df* _originalDensityGrid;
    const Grid3Di* _beforeGrid;
    const Grid3Di* _theoriticalsGrid;
    Grid3Df* _outDensityGrid;
    double _radius;
    int32_t _effectiveRayThreshold;
    int32_t _endRayThreshold;
};

#endif

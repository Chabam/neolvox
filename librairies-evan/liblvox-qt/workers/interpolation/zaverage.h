#ifndef WORKERINTERPOLATEZAVERAGE_H
#define WORKERINTERPOLATEZAVERAGE_H

#pragma once

#include <inttypes.h>

#include "../../datastructures/types.h"
#include "../workerbase.h"

class ZAverage : public WorkerBase
{
public:
    ZAverage(Grid3Df* imputDensity, Grid3Df* outputDensity, int32_t effectiveRayThreshold, int32_t numZCalculatedOn, lvox::Logger* logger);
    void run() override;

private:
    Grid3Df* _inputDensity;
    Grid3Df* _outputDensity;
    int32_t _effectiveRayThreshold;
    int32_t _numZCalculatedOn;
};

#endif

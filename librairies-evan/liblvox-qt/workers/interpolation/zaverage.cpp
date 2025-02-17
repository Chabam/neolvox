#include "zaverage.h"

ZAverage::ZAverage(Grid3Df *inputDensity, Grid3Df *outputDensity, int32_t effectiveRayThreshold, int32_t numZCalculatedOn, lvox::Logger* logger)
    : WorkerBase(logger), _inputDensity(inputDensity), _outputDensity(outputDensity), _effectiveRayThreshold(effectiveRayThreshold),
      _numZCalculatedOn(numZCalculatedOn)
{
}

void ZAverage::run()
{
    for (size_t zz = 0; zz < _inputDensity->getDimensionZ(); zz += _numZCalculatedOn) // change increment for zHeight
    {

        float meanDensity = 0;
        size_t ncells = 0;
        // Compute mean value for the level z// Loop num of zHeight
        for (int zIncrement = 0; zIncrement < _numZCalculatedOn; zIncrement++)
        {
            for (size_t yy = 0; yy < _inputDensity->getDimensionY(); yy++)
            {
                for (size_t xx = 0; xx < _inputDensity->getDimensionX(); xx++)
                {
                    float value = _inputDensity->getValueAtCoordinates(xx, yy, zz + zIncrement);
                    if (value >= 0) // empty cells don't count for the mean density
                    {
                        meanDensity += value;
                        ncells++;
                    }
                }
            }
        }
        // check ncells > 0
        meanDensity /= ncells;

        // set mean value for all NA cells
        for (int zIncrement = 0; zIncrement < _numZCalculatedOn; zIncrement++)
        {
            for (size_t yy = 0; yy < _inputDensity->getDimensionY(); yy++)
            {
                for (size_t xx = 0; xx < _inputDensity->getDimensionX(); xx++)
                {
                    float originalValue = _inputDensity->getValueAtCoordinates(xx, yy, zz + zIncrement);
                    float interpolatedValue = originalValue;
                    if (originalValue < 0 && originalValue > -7.9) // replace NA values or incoherent density results
                    {
                        interpolatedValue = meanDensity;
                    }
                    _outputDensity->setValueAtCoordinates(xx, yy, zz + zIncrement, interpolatedValue);
                }
            }
        }
    }
}

#ifndef DISTANCE_H
#define DISTANCE_H

#include "../../datastructures/types.h"
#include "../workerbase.h"

class Distance : public WorkerBase
{
public:
    Distance(const Grid3Df* grid,
             Grid3Df* resultGrid,
             double radius,
             int power,
             float densityThreshold, lvox::Logger* logger);

    void run() override;

private:
    const Grid3Df* _grid;
    Grid3Df* _resultGrid;
    double _radius;
    int _power;
    float _densityThreshold;
};

#endif // DISTANCE_H

#ifndef WORKERTHEORETICAL_H
#define WORKERTHEORETICAL_H

#include <Eigen/Geometry>
#include <vector>

#include "../workerbase.h"

#include "../../datastructures/types.h"
#include "../../tools/wootranversal.hpp"


class Theoretical : public WorkerBase
{
public:
    Theoretical(lvox::ShootingPattern* pattern, lvox::Grid3d<int>* theoreticalGrid, bool computeDistances, lvox::Logger* logger);
    void run() override;

private:
    bool _doComputeDistances;
    lvox::ShootingPattern* _shootingPattern;
    lvox::Grid3d<int>* _theoreticalGrid;
};

#endif // WORKERTHEORETICAL_H

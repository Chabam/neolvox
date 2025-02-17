#ifndef WORKERBEFORE_H
#define WORKERBEFORE_H

#pragma once

#include "../../datastructures/types.h"
#include "../../tools/wootranversal.hpp"
#include "../workerbase.h"
#include <Eigen/Geometry>

class Before : public WorkerBase
{
public:
    Before(lvox::ShootingPattern* pattern, lvox::PointCloudScene * pcs, lvox::Grid3d<int> * beforeGrid,
           bool computeDistances, lvox::Logger* logger);
    void run() override;

private:
    bool _doComputeDistances;
    lvox::Grid3d<int>* _beforeGrid;
    lvox::PointCloudScene* _pointCloudScene;
    lvox::ShootingPattern* _shootingPattern;
};

#endif

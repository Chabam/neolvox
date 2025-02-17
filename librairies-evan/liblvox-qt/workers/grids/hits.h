#ifndef WORKERHITS_H
#define WORKERHITS_H

#include "../workerbase.h"

#include "../../datastructures/types.h"
#include <Eigen/Geometry>

class __declspec(dllexport) Hits : public WorkerBase
{
public:
    Hits(lvox::ShootingPattern* pattern, lvox::PointCloudScene* pcs, lvox::Grid3d<int>* hits,
         bool computeDistances, lvox::Logger* logger = nullptr);
    void run() override;

private:
    lvox::PointCloudScene* _pointCloudScene;
    lvox::ShootingPattern* _shootingPattern;
    bool _computeDistances;
    lvox::Grid3d<int>* _hits;
};

#endif // WORKERHITS_H

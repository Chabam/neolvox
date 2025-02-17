#pragma once

#include <unordered_set>

#include "grid3d.h"
#include "pointcloudscene.h"
#include "ray.h"
#include "scanner.h"
#include "shootingpattern.h"

using Ray = lvox::Ray;
using Grid3Di = lvox::Grid3d<int>;
using Grid3Df = lvox::Grid3d<float>;
using PointCloudScene = lvox::PointCloudScene;

struct Grid3DSet
{
    Grid3Di* Ni;
    Grid3Di* Nt;
    Grid3Di* Nb;
    Grid3Df* Density;
};

struct InterpolationGrids
{
    Grid3Di* Ni;
    Grid3Di* Nt;
    Grid3Di* Nb;
    Grid3Df* DensityIn;
    Grid3Df* DensityOut;
};

using Grid3DSets = std::vector<struct Grid3DSet>;
using InterpolationGridsSet = std::unordered_set<InterpolationGrids>;

using ProgressMonitor = std::function<bool(const size_t&)>;

#ifndef COMPUTEPAD_H
#define COMPUTEPAD_H

#include <map>
#include <unordered_set>

#include "Eigen/Geometry"

#include "../datastructures/grid3d.h"
#include "../tools/logger.h"

namespace lvox
{
class ComputePAD
{

public:
    enum class GridsToCompute
    {
        BeerLamber,
        ContactFrequency,
        MaximumLikelyhoodEstimator,
        EqualBiasCorrectedBeerLamber,
        UnequalBiasCorrectedBeerLamber,
        BiasCorrectedMaximumLikelyhoodEstimator,
        IDR,
        zSum,
        zEffSum,
        deltaSum,
        deltaEffSum,
        deltaSquareSum,
        zleDelta_effSum
    };

    struct Settings
    {
        int NThreshold;
        int ErrorValue;
        bool IsRelativeThreshold;
        float GridResolution;
        int GridNAValue;
        double Lambda1;
        int Dimension;
    };

    struct Input {
        lvox::Grid3d<int>* HitGrid;
        lvox::Grid3d<int>* TheoreticalGrid;
        lvox::Grid3d<int>* BeforeGrid;
        std::vector<std::pair<GridsToCompute, lvox::Grid3d<float>*>> OutputGrids;
    };


    ComputePAD(std::vector<lvox::ComputePAD::Input>& input, Settings& settings, lvox::Logger* logger = nullptr);
    void compute();

private:

    std::vector<lvox::ComputePAD::Input>& _inputs;

    int                 _nThreshold;
    int                 _gridNAValue;
    int                 _errorValue;

    bool                _isRelativeThreshold;

    float               _gridResolution;
    double              _lambda1;

    lvox::Logger * _logger;

};
}

#endif // COMPUTEPAD_H

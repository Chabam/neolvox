#include "computepad.h"

namespace lvox
{
ComputePAD::ComputePAD(std::vector<lvox::ComputePAD::Input>& input, Settings& settings, Logger* logger) :
    _inputs(input),
    _nThreshold(settings.NThreshold),
    _errorValue(settings.ErrorValue),
    _isRelativeThreshold(settings.IsRelativeThreshold),
    _logger(logger)
{
    _gridResolution = settings.GridResolution;
    _gridNAValue = settings.GridNAValue;
    _lambda1 = settings.Lambda1;
    // std::stringstream fmt;
    // fmt << _gridResolution << " " << _gridNAValue << " " << _lambda1 << " ";
    // if(_logger != nullptr)
    //     _logger->logDebug(fmt.str());
}

void ComputePAD::compute()
{
    auto isOccluded = [&](int enteredShots, int theoreticalShots) {
        if(_isRelativeThreshold)
        {
            return enteredShots * 100.0 / theoreticalShots <= _nThreshold;
        }
        else
        {
            return enteredShots <= _nThreshold;
        }
    };
    // std::stringstream fmt;
    // fmt << "Started PAD compute method " << _inputs.size();
    if(_logger != nullptr)
        _logger->logDebug("Started PAD Computation");

    for (auto& input : _inputs)
    {
        // Unpack grids
        lvox::Grid3d<int>* theo = input.TheoreticalGrid;
        lvox::Grid3d<int>* hits = input.HitGrid;
        lvox::Grid3d<int>* befo = input.BeforeGrid;

        // std::stringstream fmt;
        // fmt << (theo == nullptr ? "true" : "false") << " "
        //     << (hits == nullptr ? "true" : "false") << " "
        //     << (befo == nullptr ? "true" : "false") << " "
        //     << input.HitGrid->cellCount() << " "
        //     << _inputs.size() << " ";

        //if(_logger != nullptr)
        //    _logger->logDebug(fmt.str());

        for(int i = 0; i < input.HitGrid->cellCount(); i++)
        {
            // std::stringstream fmt;
            int Nt = theo->getValueAtIndex(i);
            int Ni = hits->getValueAtIndex(i);
            int Nb = befo->getValueAtIndex(i);
            int N = Nt - Nb;

            // fmt << i << " " << Nt << " " << Ni << " " << Nb << " " << N << " ";

            if(Ni == 0 || isOccluded(N, Nt))
            {
                double noDataOrNoMaterialValue = _errorValue;
                if (!isOccluded(N, Nt))
                    noDataOrNoMaterialValue = 0;

                // fmt << noDataOrNoMaterialValue;
                // if(_logger != nullptr)
                //     _logger->logDebug(fmt.str());
                for(auto& e : input.OutputGrids)
                    e.second->setValueAtIndex(i, noDataOrNoMaterialValue);
            }
            else
            {
                //delta: path formed by the intersection of the ray and the voxel faces
                //z    : path formed by the intersection of the ray and objects surface z <= delta
                double deltaSum = theo->getSommationDeltaAtIndex(i) - befo->getSommationDeltaAtIndex(i);/*itemIn_deltaH->getSommaDelta(i) +*/
                double deltaEffSum = theo->getSommationEffectiveDeltaAtIndex(i) - befo->getSommationEffectiveDeltaAtIndex(i);/*itemIn_deltaH->getSommaEffectiveDelta(i) +*/
                double zSum = hits->getSommationFreeAtIndex(i) + theo->getSommationDeltaAtIndex(i)
                        - befo->getSommationDeltaAtIndex(i) - hits->getSommationDeltaAtIndex(i);
                double zEffSum = hits->getSommationEffectiveFreeAtIndex(i) + theo->getSommationEffectiveDeltaAtIndex(i)
                        - befo->getSommationEffectiveDeltaAtIndex(i) - hits->getSommationEffectiveDeltaAtIndex(i);
                double deltaSquareSum = theo->getSommationDeltaSquareAtIndex(i) - befo->getSommationDeltaSquareAtIndex(i);/*itemIn_deltaH->getSommaDeltaSquare(i)+*/
                double I = static_cast<double>(Ni) / static_cast<double>(N);
                double deltaMean = deltaSum / N;
                double freeMean = zSum / N;

                double effectiveFreePath = zEffSum / N;
                double effectiveDeltaPath = deltaEffSum / N;
                double effectiveFreePathHits = hits->getSommationEffectiveHitsAtIndex(i) / N;

                double deltaVarEff = deltaSquareSum / N - effectiveDeltaPath * effectiveDeltaPath;
                double alpha = deltaVarEff / effectiveDeltaPath;

                //Calculs des Eq.5.8.25.27.32
                // FP Modified this section to make sure inconsistent statistics lead to PAD of -1 (with G=0.5)
                double lambdaCF = I / deltaMean;
                if(deltaMean <= 0)
                    lambdaCF = _errorValue * 0.5;

                //Eq. 5
                double lambdaHat = -((log(1- I)) / deltaMean);
                if(std::isinf(lambdaHat) || deltaMean <= 0)
                    lambdaHat = _errorValue * 0.5;

                //Eq. 8
                double lambdaTilda = I / freeMean;
                if(freeMean <= 0)
                    lambdaTilda = _errorValue * 0.5;

                //Eq. 25 : Beer Lambert
                double capLambdaHat = _errorValue * 0.5;
                if(effectiveDeltaPath > 0)
                {
                    if(I < 1)
                    {
                        capLambdaHat = (-log(1 - I) - I/(2 * N * (1 - I))) / effectiveDeltaPath;
                    }
                    else if(I >= 1)
                    {
                        capLambdaHat = log(2 * N + 2) / effectiveDeltaPath;
                    }
                }

                // Eq. 27
                double capLambdaHat2 = capLambdaHat;
                if (capLambdaHat > _errorValue * 0.5)
                {
                    if (1-2*alpha*capLambdaHat >= 0.05) // case of correction
                    {
                        capLambdaHat2 = 1 / alpha * (1 - sqrt(1 - 2 * alpha * capLambdaHat));
                    }
                }

                double capLambdaTilda = 0;
                if (effectiveFreePath <= std::numeric_limits<double>::epsilon())
                {
                    capLambdaTilda = _errorValue * 0.5;
                    Eigen::Vector3d cCenter = hits->getCellCenterCoordinates(i);
                }
                else
                {
                    capLambdaTilda = I / effectiveFreePath - effectiveFreePathHits / (N * effectiveFreePath * effectiveFreePath);
                    if (capLambdaTilda < 0)
                    {
                        capLambdaTilda = 0; // force to be equal to zero if bias correction lead to negative value (this is not an error here)
                    }
                }

                // fmt << capLambdaTilda << " " << lambdaTilda << " " << capLambdaHat << " " << lambdaHat << " " << capLambdaHat2;
                // if(_logger != nullptr)
                //     _logger->logDebug(fmt.str());
                for(auto& e : input.OutputGrids)
                {
                    GridsToCompute gtc = e.first;
                    switch (gtc) {
                    case GridsToCompute::BeerLamber:
                        e.second->setValueAtIndex(i, lambdaHat * (1 / 0.5));
                        break;
                    case GridsToCompute::ContactFrequency:
                        e.second->setValueAtIndex(i, lambdaCF * (1 / 0.5));
                        break;
                    case GridsToCompute::MaximumLikelyhoodEstimator:
                        e.second->setValueAtIndex(i, lambdaTilda * (1 / 0.5));
                        break;
                    case GridsToCompute::EqualBiasCorrectedBeerLamber:
                        e.second->setValueAtIndex(i, capLambdaHat * (1 / 0.5));
                        break;
                    case GridsToCompute::UnequalBiasCorrectedBeerLamber:
                        e.second->setValueAtIndex(i, capLambdaHat2 * (1 / 0.5));
                        break;
                    case GridsToCompute::BiasCorrectedMaximumLikelyhoodEstimator:
                        e.second->setValueAtIndex(i, capLambdaTilda * (1 / 0.5));
                        break;
                    case GridsToCompute::zSum:
                        e.second->setValueAtIndex(i, zSum);
                        break;
                    case GridsToCompute::zEffSum:
                        e.second->setValueAtIndex(i, zEffSum);
                        break;
                    case GridsToCompute::deltaSum:
                        e.second->setValueAtIndex(i, deltaSum);
                        break;
                    case GridsToCompute::deltaEffSum:
                        e.second->setValueAtIndex(i, deltaEffSum);
                        break;
                    case GridsToCompute::deltaSquareSum:
                        e.second->setValueAtIndex(i, deltaSquareSum);
                        break;
                    case GridsToCompute::zleDelta_effSum:
                        e.second->setValueAtIndex(i, hits->getSommationEffectiveHitsAtIndex(i));
                        break;
                    case GridsToCompute::IDR:
                        e.second->setValueAtIndex(i, I);
                        break;
                    }
                }
            }
        }
    }
}
}

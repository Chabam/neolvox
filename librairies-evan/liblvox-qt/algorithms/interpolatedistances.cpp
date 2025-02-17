#include "interpolatedistances.h"

#include "../workers/workerbase.h"
#include "../workers/interpolation/distance.h"
#include "../workers/interpolation/trustfactor.h"
#include "../workers/interpolation/zaverage.h"


InterpolateDistances::InterpolateDistances(std::vector<Input>& sets, Method method, Settings settings, lvox::Logger* logger) :
    _sets(sets), _method(method), _settings(settings), _logger(logger)
{

}

void InterpolateDistances::compute()
{
    std::vector<WorkerBase*> workerPool;

    for (auto& set : _sets)
    {
        WorkerBase * worker;
        switch(_method)
        {
        case Method::Trust:
            worker = new TrustFactor(set.DensityIn, set.Nb, set.Nt, set.DensityOut,
                                     _settings.InterpolateRadius, _settings.TrustLowThreshold, _settings.TrustHighThreshold, _logger);
            break;
        case Method::Distance:
            worker = new Distance(set.DensityIn, set.DensityOut, _settings.InterpolateRadius, _settings.InterpolatePower, _settings.InterpolateDensityThreshold, _logger);
            break;
        case Method::ZAverage:
            worker = new ZAverage(set.DensityIn, set.DensityOut, _settings.EffectiveRayThresh, _settings.NumZCalculatedOn, _logger);
            break;
        }
        workerPool.push_back(worker);
    }

    // TODO parallel
    for(auto worker : workerPool)
        worker->run();
}


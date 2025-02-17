#include "theoretical.h"
#include <algorithm>

Theoretical::Theoretical(lvox::ShootingPattern* pattern, lvox::Grid3d<int>* theoreticalGrid, bool computeDistances, lvox::Logger* logger)
    : WorkerBase(logger), _doComputeDistances(computeDistances), _shootingPattern(pattern), _theoreticalGrid(theoreticalGrid)
{
}

void Theoretical::run()
{
    WooTraversal<int>::VisitorFunction visitorFunction;

    if (_doComputeDistances)
    {
        visitorFunction = WooTraversal<int>::DistanceVisitor;
    }
    else
    {
        visitorFunction = WooTraversal<int>::CountVisitor;
    }

    // Creates traversal algorithm
    WooTraversal<int> algo(*_theoreticalGrid, visitorFunction, true, _logger);

    const size_t nShot = _shootingPattern->getNumberOfShots();
    // _logger.logInfo(std::to_string(nShot));

    for (size_t i = 0; i < nShot; ++i)
    {
        lvox::Ray ray = _shootingPattern->shotAt(i);

        //_theoreticalGrid.addValueAtIndex(0, 1);
        // std::stringstream fmt;
        //fmt << ray.getOrigin()(0) << " " << ray.getOrigin()(1) << " " << ray.getOrigin()(2) << " "
        //    << ray.getDirection()(0) << " " << ray.getDirection()(1) << " " << ray.getDirection()(2) << " "
        //    << origin(0) << " " << origin(1) << " " << origin(2) << " "
        //    << direction(0) << " " << direction(1) << " " << direction(2);

        //_logger.logDebug(fmt.str());
        algo.traverse(ray);
    }
}

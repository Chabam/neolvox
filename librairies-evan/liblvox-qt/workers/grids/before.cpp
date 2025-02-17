#include "before.h"

Before::Before(lvox::ShootingPattern* pattern, lvox::PointCloudScene* pcs, lvox::Grid3d<int>* beforeGrid,
               bool computeDistances, lvox::Logger* logger)
    : WorkerBase(logger), _doComputeDistances(computeDistances), _beforeGrid(beforeGrid), _pointCloudScene(pcs), _shootingPattern(pattern)
{
}

void Before::run()
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
    WooTraversal<int> algo(*_beforeGrid, visitorFunction, false, _logger);

    const size_t nShot = _shootingPattern->getNumberOfShots();
    for (size_t i = 0; i < nShot; ++i)
    {
        // Eigen::Vector3d point = _pointCloudScene->getPointByIndex(i);
        // Eigen::Vector3d shotOrigin = _shootingPattern->getRayForPoint(point).getOrigin();
        // auto direction = (point - shotOrigin).normalized();

        Eigen::Vector3d point = _shootingPattern->shotAt(i).getOrigin() + _shootingPattern->shotAt(i).getDirection() ;
        auto direction = _shootingPattern->shotAt(i).getDirection().normalized();

        const lvox::Ray ray{point, direction};

//        std::stringstream fmt;
//        fmt << point(0) << " " << point(1) << " " << point(2) << " "
//            << shotOrigin(0) << " " << shotOrigin(1) << " " << shotOrigin(2) << " "
//            << direction(0) << " " << direction(1) << " " << direction(2);
//        _logger.logDebug(fmt.str());
        algo.traverse(ray);
    }
}

#include "lvox3_implshootingpattern.h"
#include "tools/lvox3_logger.h"

LVOX3_ImplShootingPattern::LVOX3_ImplShootingPattern(CT_ShootingPattern * shootingPattern): _shootingPattern(shootingPattern)
{
    LVOX3_Logger log;
    log.logInfo("Created ImplShootingPattern : " +
                std::to_string(_shootingPattern->centerCoordinate().x()) + " " + std::to_string(_shootingPattern->centerCoordinate().y()) + " " + std::to_string(_shootingPattern->centerCoordinate().z()));
}

const Eigen::Vector3d& LVOX3_ImplShootingPattern::getCenterCoordinate() const
{
    LVOX3_Logger log;
    log.logInfo("Requested : " +
                std::to_string(_shootingPattern->centerCoordinate().x()) + " " + std::to_string(_shootingPattern->centerCoordinate().y())
                + " " + std::to_string(_shootingPattern->centerCoordinate().z()));
    return _shootingPattern->centerCoordinate();
}

size_t LVOX3_ImplShootingPattern::getNumberOfShots() const
{
    return _shootingPattern->numberOfShots();
}

lvox::Ray LVOX3_ImplShootingPattern::getRayForPoint(const Eigen::Vector3d& point) const
{
    const CT_Point pt{point};
    CT_Shot shot = _shootingPattern->shotForPoint(pt);
    return lvox::Ray{shot.origin(), shot.direction()};
}

lvox::Ray LVOX3_ImplShootingPattern::shotAt(size_t index) const
{
    CT_Shot shot = _shootingPattern->shotAt(index);
    lvox::Ray ray = lvox::Ray{ shot.origin(), shot.direction().normalized()};

    //std::stringstream fmt;
    //fmt << ray.getOrigin()(0) << " " << ray.getOrigin()(1) << " " << ray.getOrigin()(2) << " "
    //    << ray.getDirection()(0) << " " << ray.getDirection()(1) << " " << ray.getDirection()(2);

    //qDebug() << fmt.str().c_str();
    return ray;
}


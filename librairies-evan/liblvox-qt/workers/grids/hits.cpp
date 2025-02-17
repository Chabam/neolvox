#include "hits.h"


Hits::Hits(lvox::ShootingPattern* pattern, lvox::PointCloudScene* pcs, lvox::Grid3d<int>* hits,
           bool computeDistances, lvox::Logger* logger)
    : WorkerBase(logger), _pointCloudScene(pcs), _shootingPattern(pattern), _computeDistances(computeDistances), _hits(hits)
{
}

void Hits::run()
{
    Eigen::Vector3d bottom, top, in, out;

    const size_t n_points = _pointCloudScene->getSize();

    bool computeDistance = _computeDistances;


    for (size_t pointIndice = 0; pointIndice < n_points; pointIndice++)
    {
        // Eigen::Vector3d point2 = _pointCloudScene->getPointByIndex(pointIndice);

        Eigen::Vector3d point_orig = _shootingPattern->shotAt(pointIndice).getOrigin();
        Eigen::Vector3d point_dir = _shootingPattern->shotAt(pointIndice).getDirection();

        Eigen::Vector3d point = point_orig + point_dir;


        // Formater les coordonnées des points
        // std::stringstream fmt;
        // fmt << "1: " << point(0) << " " << point(1) << " " << point(2) << "\n";
        // fmt << "2: " << point2(0) << " " << point2(1) << " " << point2(2) << "\n";

        // if(pointIndice%1000000 == 0) {
        //     _logger->logInfo(fmt.str());
        // }

        //_hits->addValueAtCoordinates(point.x(), point.y(), point.z(), 1);

        // we now that the grid is perfectly bounding the scene so we can use this tools that don't do
        // many check to reduce the compute time !
        // FP 11/2017 NOT TRUE: we should check if the point is in the grid first thanks to the following test
        if ( point.x() >= _hits->getMinX() && point.x() < _hits->getMaxX() &&
             point.y() >= _hits->getMinY() && point.y() < _hits->getMaxY() &&
             point.z() >= _hits->getMinZ() && point.z() < _hits->getMaxZ())
        {
            size_t col;
            size_t lin;
            size_t lev;
            size_t ind;

            _hits->computeGridIndexForPoint(point, col, lin, lev, ind);

            // fmt << col << " " << lin << " " << lev << " " << ind << " ";


            if (!_hits->isFiltered(_hits->getValueAtIndex(ind)))
            {
                if (computeDistance)
                {
                    _hits->computeCellBottomLeftTopRightCornerAtColLinLevel(col, lin, lev, bottom, top);

                    // lvox::Ray shot = _shootingPattern->getRayForPoint(point);
                    lvox::Ray shot = _shootingPattern->shotAt(pointIndice);

                    // if (shot.getDirection() != shot2.getDirection()){
                    //     fmt << "not same" ;
                    //     _logger->logInfo(fmt.str());
                    // }


                    // fmt << shot.getOrigin()(0) << " " << shot.getOrigin()(1) << " " <<shot.getOrigin()(2) << " "
                    //     << shot.getDirection()(0) << " " << shot.getDirection()(1) << " " << shot.getDirection()(2) << " ";

                    if (shot.intersect(bottom, top, in, out))
                    {
                        // fmt << in(0) << " " << in(1) << " " << in(2) << " "
                        //     << out(0) << " " << out(1) << " " << out(2) << " ";
                        _hits->doSommation(ind, (in - point).norm() + (out - point).norm(), (in - point).norm());
                        _hits->addValueAtIndex(ind, 1);
                    }
                }
                else
                {
                    _hits->addValueAtIndex(ind, 1);
                }
            }
        }
        // if(pointIndice%100000 == 0) {
        //     _logger->logInfo(fmt.str());
        // }
    }
}

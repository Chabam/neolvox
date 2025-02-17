#include "lvox3_implpointcloud.h"

LVOX3_ImplPointCloud::LVOX3_ImplPointCloud(const CT_Scene* scene) : _scene(scene)
{
    _index = scene->pointCloudIndex();
}

Eigen::Vector3d LVOX3_ImplPointCloud::getPointByIndex(size_t index)
{
    CT_PointIterator itP(_index);
    itP.jump(index);
    Eigen::Vector3d currentPoint = itP.currentPoint();
    //qDebug() << "Requested point" << currentPoint(0) << " " << currentPoint(1) << " " << currentPoint(2);
    return currentPoint;
}

void LVOX3_ImplPointCloud::boundingBox(Eigen::Vector3d& _outSceneBoundingBoxMinCoordinates,
                         Eigen::Vector3d& _outSceneBoundingBoxMaxCoordinates)
{
    _outSceneBoundingBoxMinCoordinates = _scene->minCoordinates();
    _outSceneBoundingBoxMaxCoordinates = _scene->maxCoordinates();
}



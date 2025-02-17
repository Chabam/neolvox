#ifndef LVOX3_IMPLPOINTCLOUD_H
#define LVOX3_IMPLPOINTCLOUD_H

#include "ct_itemdrawable/ct_scene.h"
#include <liblvox/datastructures/pointcloudscene.h>

class LVOX3_ImplPointCloud : public lvox::PointCloudScene
{

public:
    LVOX3_ImplPointCloud(const CT_Scene* scene);
    int getSize() override { return _scene->pointCloudIndexSize(); };
    Eigen::Vector3d getPointByIndex(size_t index) override;
    void boundingBox(Eigen::Vector3d& _outSceneBoundingBoxMinCoordinates,
                             Eigen::Vector3d& _outSceneBoundingBoxMaxCoordinates) override;
private:
    const CT_Scene * _scene;
    const CT_AbstractPointCloudIndex * _index;
};

#endif // LVOX3_IMPLPOINTCLOUD_H

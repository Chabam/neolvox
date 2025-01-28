#include "ct_internalpointcloud.h"

CT_InternalPointCloud::CT_InternalPointCloud(const size_t &initialSize) : CT_StandardCloudStdVectorT<CT_PointData>(initialSize)
{
}

CT_AbstractCloud *CT_InternalPointCloud::copy() const
{
    size_t s = size();

    CT_InternalPointCloud *cloud = new CT_InternalPointCloud(s);

    for(size_t i=0; i<s; ++i)
        (*cloud)[i] = (*this)[i];

    return cloud;
}

#ifndef CT_INTERNALPOINTCLOUD_H
#define CT_INTERNALPOINTCLOUD_H

#include "ct_cloud/ct_standardcloudstdvectort.h"
#include "ct_point.h"

class CTLIBCLOUDS_EXPORT CT_InternalPointCloud : public CT_StandardCloudStdVectorT<CT_PointData>
{
public:
    CT_InternalPointCloud(const size_t &initialSize = 0);

    virtual CT_AbstractCloud* copy() const;
};

#endif // CT_INTERNALPOINTCLOUD_H

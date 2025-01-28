#include "ct_gdalheader.h"

CT_DEFAULT_IA_INIT(CT_GDALHeader)

CT_GDALHeader::CT_GDALHeader() : SuperClass()
{
}

void CT_GDALHeader::setBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max)
{
    m_minCoordinates = min;
    m_maxCoordinates = max;
}

void CT_GDALHeader::boundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const
{
    min = m_minCoordinates;
    max = m_maxCoordinates;
}

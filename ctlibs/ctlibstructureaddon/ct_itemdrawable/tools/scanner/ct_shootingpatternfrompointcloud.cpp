#include "ct_shootingpatternfrompointcloud.h"

#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"

CT_ShootingPatternFromPointCloud::CT_ShootingPatternFromPointCloud() : CT_ShootingPattern(),
    m_origin(0, 0, 0),
    m_pcir(nullptr)
{
}

CT_ShootingPatternFromPointCloud::CT_ShootingPatternFromPointCloud(const Eigen::Vector3d& origin, CT_PCIR pcir) : CT_ShootingPattern(),
    m_origin(origin),
    m_pcir(pcir)
{
}

CT_ShootingPatternFromPointCloud::CT_ShootingPatternFromPointCloud(const CT_ShootingPatternFromPointCloud &other) : CT_ShootingPattern(),
    m_origin(other.m_origin),
    m_pcir(other.m_pcir)
{
}

size_t CT_ShootingPatternFromPointCloud::numberOfShots() const
{
    return m_pcir->abstractCloudIndexT()->size();
}

CT_Shot CT_ShootingPatternFromPointCloud::shotAt(const size_t &index)
{
    auto idx = m_pcir->abstractCloudIndexT()->constIndexAt(index);
    const CT_Point& p = m_pAccessor.pointAt(idx);

    auto direction = p - m_origin;
    return CT_Shot(m_origin, direction);
}

CT_Shot CT_ShootingPatternFromPointCloud::shotForPoint(const CT_Point &pt)
{
    return CT_Shot(m_origin, pt - m_origin);
}

CT_ShootingPattern* CT_ShootingPatternFromPointCloud::clone() const
{
    return new CT_ShootingPatternFromPointCloud(*this);
}

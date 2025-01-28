#include "ct_spheredata.h"

CT_SphereData::CT_SphereData() : SuperClass()
{
    m_radius = 0;
}

CT_SphereData::CT_SphereData(const Eigen::Vector3d& center, double radius) : SuperClass(center, Eigen::Vector3d(0, 0, 0))
{
    m_radius = radius;
}

double CT_SphereData::getRadius() const
{
    return m_radius;
}

void CT_SphereData::getBoundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    min(0) = _center(0) - m_radius;
    min(1) = _center(1) - m_radius;
    min(2) = _center(2) - m_radius;

    max(0) = _center(0) + m_radius;
    max(1) = _center(1) + m_radius;
    max(2) = _center(2) + m_radius;
}

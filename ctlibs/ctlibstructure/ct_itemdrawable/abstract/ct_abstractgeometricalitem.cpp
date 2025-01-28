#include "ct_abstractgeometricalitem.h"

CT_DEFAULT_IA_INIT(CT_AbstractGeometricalItem);

CT_AbstractGeometricalItem::CT_AbstractGeometricalItem() : SuperClass()
{
    m_centerCoordinates(0) = 0;
    m_centerCoordinates(1) = 0;
    m_centerCoordinates(2) = 0;

    m_minCoordinates(0) = std::numeric_limits<double>::max();
    m_minCoordinates(1) = std::numeric_limits<double>::max();
    m_minCoordinates(2) = std::numeric_limits<double>::max();

    m_maxCoordinates(0) = -std::numeric_limits<double>::max();
    m_maxCoordinates(1) = -std::numeric_limits<double>::max();
    m_maxCoordinates(2) = -std::numeric_limits<double>::max();
}

void CT_AbstractGeometricalItem::setCenterX(double x)
{
    m_centerCoordinates(0) = x;
}

void CT_AbstractGeometricalItem::setCenterY(double y)
{
    m_centerCoordinates(1) = y;
}

void CT_AbstractGeometricalItem::setCenterZ(double z)
{
    m_centerCoordinates(2) = z;
}

double CT_AbstractGeometricalItem::centerX() const
{
    return m_centerCoordinates(0);
}

double CT_AbstractGeometricalItem::centerY() const
{
    return m_centerCoordinates(1);
}

double CT_AbstractGeometricalItem::centerZ() const
{
    return m_centerCoordinates(2);
}

void CT_AbstractGeometricalItem::boundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    min = m_minCoordinates;
    max = m_maxCoordinates;
}

void CT_AbstractGeometricalItem::scaledBoundingBox(double xCoeff, double yCoeff, double zCoeff, Eigen::Vector3d &min, Eigen::Vector3d &max)
{
    boundingBox(min, max);

    min(0) = centerX() - (fabs(min.x()-centerX()) * xCoeff);
    min(1) = centerY() - (fabs(min.y()-centerY()) * yCoeff);
    min(2) = centerZ() - (fabs(min.z()-centerZ()) * zCoeff);

    max(0) = centerX() + (fabs(max.x()-centerX()) * xCoeff);
    max(1) = centerY() + (fabs(max.y()-centerY()) * yCoeff);
    max(2) = centerZ() + (fabs(max.z()-centerZ()) * zCoeff);
}

void CT_AbstractGeometricalItem::bufferedBoundingBox(double xMeters, double yMeters, double zMeters, Eigen::Vector3d &min, Eigen::Vector3d &max)
{
    boundingBox(min, max);

    min(0) -= xMeters;
    min(1) -= yMeters;
    min(2) -= zMeters;

    max(0) += xMeters;
    max(1) += yMeters;
    max(2) += zMeters;
}

void CT_AbstractGeometricalItem::setBoundingBox(double minx, double miny, double minz, double maxx, double maxy, double maxz)
{
    m_minCoordinates(0) = minx;
    m_minCoordinates(1) = miny;
    m_minCoordinates(2) = minz;

    m_maxCoordinates(0) = maxx;
    m_maxCoordinates(1) = maxy;
    m_maxCoordinates(2) = maxz;

    updateCenterFromBoundingBox();
}

void CT_AbstractGeometricalItem::setBoundingBox(const Eigen::Vector3d& min, const Eigen::Vector3d& max)
{
    m_minCoordinates = min;
    m_maxCoordinates = max;

    updateCenterFromBoundingBox();
}

void CT_AbstractGeometricalItem::updateCenterFromBoundingBox()
{
    m_centerCoordinates = (m_maxCoordinates + m_minCoordinates) / 2.0;
}

Eigen::Vector3d CT_AbstractGeometricalItem::minCoordinates() const
{
    return m_minCoordinates;
}

Eigen::Vector3d CT_AbstractGeometricalItem::maxCoordinates() const
{
    return m_maxCoordinates;
}

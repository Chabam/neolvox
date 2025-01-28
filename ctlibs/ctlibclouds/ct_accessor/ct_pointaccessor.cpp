#include "ct_pointaccessor.h"

#include "ct_global/ct_cloudscontext.h"
#include "ct_global/ct_repositorymanager.h"
#include "ct_global/ct_repository.h"
#include "ct_pointcloud/abstract/ct_abstractpointcloud.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"

CT_PointAccessor::CT_PointAccessor()
{
    m_pCloud = PS_REPOSITORY->globalPointCloud();
}

CT_Point CT_PointAccessor::pointAt(const size_t &globalIndex) const
{
    CT_Point p;

    m_pCloud->constTAt(globalIndex).realPoint(p, globalIndex);
    return p;
}

void CT_PointAccessor::pointAt(const size_t &globalIndex, CT_Point &point) const
{
    m_pCloud->constTAt(globalIndex).realPoint(point, globalIndex);
}

const CT_Point& CT_PointAccessor::constPointAt(const size_t &globalIndex) const
{
    m_pCloud->constTAt(globalIndex).realPoint(m_p, globalIndex);
    return m_p;
}

void CT_PointAccessor::replacePointAt(const size_t &globalIndex, const CT_Point &p)
{
    // get the internal point
    CT_PointData &pData = internalPointAt(globalIndex);

    // get the coordinate system manager
    CT_CoordinateSystemManager *csm = PS_COORDINATES_SYS_MANAGER;

    GLuint csIndex;

    // compute the coordinate system to use
    CT_AbstractCoordinateSystem *coordinateSystem = csm->computeCoordinateSystemForPointAndAddItToCollection(p, csIndex);

    // set for this point it's index
    csm->setCoordinateSystemForPointAt(globalIndex, csIndex);

    // convert the point
    coordinateSystem->convertImport(p(CT_Point::X), p(CT_Point::Y), p(CT_Point::Z), pData(CT_PointData::X), pData(CT_PointData::Y), pData(CT_PointData::Z));
}

CT_PointData& CT_PointAccessor::internalPointAt(const size_t &globalIndex)
{
    return m_pCloud->tAt(globalIndex);
}

const CT_PointData& CT_PointAccessor::constInternalPointAt(const size_t &globalIndex) const
{
    return m_pCloud->constTAt(globalIndex);
}

void CT_PointAccessor::replaceInternalPointAt(const size_t &globalIndex, const CT_PointData &p)
{
    // get the internal point
    CT_PointData &pData = internalPointAt(globalIndex);

    // change it
    pData = p;
}

CT_AbstractPointCloud* CT_PointAccessor::cloud() const
{
    return m_pCloud;
}

size_t CT_PointAccessor::size() const
{
    return m_pCloud->size();
}

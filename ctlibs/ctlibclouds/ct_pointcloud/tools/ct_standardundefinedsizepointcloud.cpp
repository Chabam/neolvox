#include "ct_standardundefinedsizepointcloud.h"

#include "ct_global/ct_cloudscontext.h"
#include "ct_point.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"

size_t CT_StandardUndefinedSizePointCloud::beginIndex() const
{
    return m_bIndex;
}

size_t CT_StandardUndefinedSizePointCloud::lastIndex() const
{
    size_t s = size();

    if(s == 0)
        return m_bIndex;

    return m_bIndex+(s-1);
}

size_t CT_StandardUndefinedSizePointCloud::size() const
{
    return m_pc->size()-m_bIndex;
}

void CT_StandardUndefinedSizePointCloud::addPoint(const CT_Point &p)
{
    // get the internal point
    CT_PointData &pData = m_pc->addT();

    GLuint csIndex;

    // compute the coordinate system to use
    CT_AbstractCoordinateSystem *coordinateSystem = m_csm->computeCoordinateSystemForPointAndAddItToCollection(p, csIndex);

    // backup for this point it's coordinate system index
    m_csIndexes.push_back(csIndex);

    // convert the point
    coordinateSystem->convertImport(p(CT_Point::X), p(CT_Point::Y), p(CT_Point::Z), pData(CT_PointData::X), pData(CT_PointData::Y), pData(CT_PointData::Z));
}

void CT_StandardUndefinedSizePointCloud::addInternalPoint(const CT_PointData &point)
{
    m_pc->addT(point);
    m_csIndexes.push_back(0);
}

void CT_StandardUndefinedSizePointCloud::addInternalPoint(const CT_PointData &point, const GLuint &csIndex)
{
    m_pc->addT(point);
    m_csIndexes.push_back(csIndex);
}

CT_StandardUndefinedSizePointCloud::CT_StandardUndefinedSizePointCloud(const size_t &beginIndex, const CT_InternalPointCloud *cloud) : CT_AbstractUndefinedSizePointCloud()
{
    m_bIndex = beginIndex;
    m_pc = (CT_InternalPointCloud*)cloud;
    m_csm = PS_COORDINATES_SYS_MANAGER;
}

const std::vector<GLuint>& CT_StandardUndefinedSizePointCloud::coordinateSystemIndexOfPointsAdded() const
{
    return m_csIndexes;
}

#include "ct_mutablepointiterator.h"

#include "ct_global/ct_cloudscontext.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"

CT_MutablePointIterator::CT_MutablePointIterator(const CT_AbstractPointCloudIndex *pci)
{
    m_it = new CT_PointIterator(pci);

    // get the coordinate system manager
    m_csm = PS_COORDINATES_SYS_MANAGER;
}

CT_MutablePointIterator::CT_MutablePointIterator(CT_CIR cir)
{
    m_it = new CT_PointIterator(cir);

    // get the coordinate system manager
    m_csm = PS_COORDINATES_SYS_MANAGER;
}

CT_MutablePointIterator::CT_MutablePointIterator(const CT_MutablePointIterator &it)
{
    if(it.m_it == nullptr)
        m_it = nullptr;
    else
        m_it = new CT_PointIterator(*it.m_it);

    // get the coordinate system manager
    m_csm = PS_COORDINATES_SYS_MANAGER;
}

CT_MutablePointIterator::~CT_MutablePointIterator()
{
    delete m_it;
}

bool CT_MutablePointIterator::hasNext() const
{
    return m_it->hasNext();
}

CT_MutablePointIterator& CT_MutablePointIterator::next()
{
    m_it->next();
    return *this;
}

bool CT_MutablePointIterator::hasPrevious() const
{
    return m_it->hasPrevious();
}

CT_MutablePointIterator& CT_MutablePointIterator::previous()
{
    m_it->previous();
    return *this;
}

CT_MutablePointIterator& CT_MutablePointIterator::jump(size_t n)
{
    m_it->jump(n);
    return *this;
}

void CT_MutablePointIterator::toFront()
{
    m_it->toFront();
}

void CT_MutablePointIterator::toBack()
{
    m_it->toBack();
}

size_t CT_MutablePointIterator::size() const
{
    return m_it->size();
}

size_t CT_MutablePointIterator::currentGlobalIndex() const
{
    return m_it->currentGlobalIndex();
}

CT_AbstractCoordinateSystem* CT_MutablePointIterator::currentCoordinateSystem() const
{
    return m_it->currentCoordinateSystem();
}

GLuint CT_MutablePointIterator::currentCoordinateSystemIndex() const
{
    return m_it->currentCoordinateSystemIndex();
}

const CT_Point &CT_MutablePointIterator::currentPoint() const
{
    return m_it->currentPoint();
}

void CT_MutablePointIterator::replaceCurrentPoint(const CT_Point &p)
{
    // get the internal point
    CT_PointData &pData = m_it->currentInternalPoint();

    GLuint csIndex;

    // compute the coordinate system to use
    CT_AbstractCoordinateSystem *coordinateSystem = m_csm->computeCoordinateSystemForPointAndAddItToCollection(p, csIndex);

    // set for this point it's index
    m_csm->setCoordinateSystemForPointAt(m_it->currentGlobalIndex(), csIndex);

    // convert the point
    coordinateSystem->convertImport(p(CT_Point::X), p(CT_Point::Y), p(CT_Point::Z), pData(CT_PointData::X), pData(CT_PointData::Y), pData(CT_PointData::Z));
}

CT_PointData& CT_MutablePointIterator::currentInternalPoint()
{
    return m_it->currentInternalPoint();
}

const CT_PointData& CT_MutablePointIterator::currentConstInternalPoint() const
{
    return m_it->currentConstInternalPoint();
}

void CT_MutablePointIterator::replaceCurrentInternalPoint(const CT_PointData &p)
{
    CT_PointData &pData = m_it->currentInternalPoint();
    pData = p;
}

void CT_MutablePointIterator::replaceCurrentInternalPoint(const CT_PointData &p, const GLuint &csIndex)
{
    CT_PointData &pData = m_it->currentInternalPoint();
    pData = p;

    // set for this point it's index
    m_csm->setCoordinateSystemForPointAt(m_it->currentGlobalIndex(), csIndex);
}

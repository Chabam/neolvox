#include "ct_pointiterator.h"

#include "ct_cloudindex/abstract/ct_abstractcloudindext.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"

#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"

#include "ct_global/ct_cloudscontext.h"

CT_PointIterator::CT_PointIterator(const CT_AbstractPointCloudIndex *pci)
{
    init(pci);
}

CT_PointIterator::CT_PointIterator(CT_CIR cir)
{
    if(cir.data() == nullptr)
        init(nullptr);
    else
        init(dynamic_cast<CT_AbstractPointCloudIndex*>(cir->abstractCloudIndex()));
}

CT_PointIterator::CT_PointIterator(const CT_PointIterator &it)
{
    if(it.m_it == nullptr)
        m_it = nullptr;
    else
        m_it = new CT_CloudIndexIteratorT<CT_PointData>(*it.m_it);
}

CT_PointIterator::~CT_PointIterator()
{
    delete m_it;
}

bool CT_PointIterator::hasNext() const
{
    if(m_it == nullptr)
        return false;

    return m_it->hasNext();
}

CT_PointIterator& CT_PointIterator::next()
{
    m_it->next();
    return *this;
}

bool CT_PointIterator::hasPrevious() const
{
    if(m_it == nullptr)
        return false;

    return m_it->hasPrevious();
}

CT_PointIterator& CT_PointIterator::previous()
{
    m_it->previous();
    return *this;
}

void CT_PointIterator::jump(size_t n)
{
    if(m_it != nullptr)
        m_it->jump(n);
}

void CT_PointIterator::toFront()
{
    if(m_it != nullptr)
        m_it->toFront();
}

void CT_PointIterator::toBack()
{
    if(m_it != nullptr)
        m_it->toBack();
}

size_t CT_PointIterator::size() const
{
    if(m_it == nullptr)
        return 0;

    return m_it->size();
}

const CT_Point& CT_PointIterator::currentPoint() const
{
    const CT_PointData &p = currentConstInternalPoint();

    p.realPoint(m_p, currentGlobalIndex());

    return m_p;
}

size_t CT_PointIterator::currentGlobalIndex() const
{
    return m_it->cIndex();
}

CT_AbstractCoordinateSystem* CT_PointIterator::currentCoordinateSystem() const
{
    return PS_COORDINATES_SYS_MANAGER->coordinateSystemForPointAt(currentGlobalIndex());
}

GLuint CT_PointIterator::currentCoordinateSystemIndex() const
{
    return PS_COORDINATES_SYS_MANAGER->coordinateSystemIndexForPointAt(currentGlobalIndex());
}

CT_PointData& CT_PointIterator::currentInternalPoint()
{
    return m_it->cT();
}

const CT_PointData& CT_PointIterator::currentConstInternalPoint() const
{
    return m_it->cT();
}

void CT_PointIterator::init(const CT_AbstractPointCloudIndex *pci)
{
    m_it = nullptr;

    if(pci != nullptr) {
        size_t s = pci->size();

        if(s > 0) {
            m_it = new CT_CloudIndexIteratorT<CT_PointData>(pci);
        }
    }
}

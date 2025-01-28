#include "ct_abstractfilter_xyz.h"

#include "ct_iterator/ct_pointiterator.h"
#include "ct_log/ct_logmanager.h"

CT_AbstractFilter_XYZ::CT_AbstractFilter_XYZ(QString pluginName) : CT_AbstractFilter(pluginName)
{
    _inCloud = nullptr;
    _outCloud = new CT_PointCloudIndexVector();
    m_minMaxUpdated = true;
}

CT_AbstractFilter_XYZ::CT_AbstractFilter_XYZ(const CT_AbstractFilter_XYZ &other) : CT_AbstractFilter(other)
{
    _inCloud = other._inCloud;
    _outCloud = dynamic_cast<CT_PointCloudIndexVector*>(other._outCloud->copy());
    m_min = other.m_min;
    m_max = other.m_max;
    m_minMaxUpdated = other.m_minMaxUpdated;
}

CT_AbstractFilter_XYZ::~CT_AbstractFilter_XYZ()
{
    delete _outCloud;
}

bool CT_AbstractFilter_XYZ::setPointCloud(const CT_AbstractItemDrawableWithPointCloud *inItem)
{
    bool ret = setPointCloud((inItem == nullptr) ? nullptr : inItem->pointCloudIndex());

    if(ret && (inItem != nullptr)) {
        inItem->boundingBox(m_min, m_max);
        m_minMaxUpdated = true;
    }

    return ret;
}

bool CT_AbstractFilter_XYZ::setPointCloud(const CT_AbstractPointCloudIndex *ci)
{
    m_minMaxUpdated = false;
    _inCloud = const_cast<CT_AbstractPointCloudIndex*>(ci);

    return (_inCloud != nullptr);
}

void CT_AbstractFilter_XYZ::updateAll()
{
    updateMinMax();
}

bool CT_AbstractFilter_XYZ::filterPointCloudIndex()
{
    if(_inCloud == nullptr)
        return false;

    if(_outCloud == nullptr)
        _outCloud = new CT_PointCloudIndexVector();

    _outCloud->clear();

    size_t nPoints = 0, nPointsKeept = 0;

    CT_PointIterator itP(_inCloud);

    while(itP.hasNext())
    {
        if(validatePoint(itP.next())) {
            _outCloud->addIndex(itP.currentGlobalIndex());
            ++nPointsKeept;
        }

        ++nPoints;
    }

    PS_LOG->addInfoMessage(LogInterface::filter, tr("%1 points filtrés sur %2 points").arg(nPoints-nPointsKeept).arg(nPoints));

    return true;
}

CT_PointCloudIndexVector* CT_AbstractFilter_XYZ::takeOutputCloudIndex()
{
    CT_PointCloudIndexVector *out = _outCloud;
    _outCloud = nullptr;

    return out;
}

bool CT_AbstractFilter_XYZ::isMinAndMaxUpdated() const
{
    return m_minMaxUpdated;
}

Eigen::Vector3d CT_AbstractFilter_XYZ::minBBox() const
{
    return m_min;
}


Eigen::Vector3d CT_AbstractFilter_XYZ::maxBBox() const
{
    return m_max;
}

bool CT_AbstractFilter_XYZ::updateMinMax(bool force)
{
    m_minMaxUpdated = false;

    if(_inCloud == nullptr)
        return false;

    if(!isMinAndMaxUpdated() || force) {
        m_min.setConstant(3, std::numeric_limits<double>::max());
        m_max.setConstant(3, m_min[0]);

        CT_PointIterator itP(_inCloud);

        while(itP.hasNext())
        {
            const CT_Point &p = itP.currentPoint();

            m_min[0] = qMin(p(CT_Point::X), m_min[0]);
            m_min[1] = qMin(p(CT_Point::Y), m_min[1]);
            m_min[2] = qMin(p(CT_Point::Z), m_min[2]);

            m_max[0] = qMax(p(CT_Point::X), m_max[0]);
            m_max[1] = qMax(p(CT_Point::Y), m_max[1]);
            m_max[2] = qMax(p(CT_Point::Z), m_max[2]);
        }

        m_minMaxUpdated = true;
    }

    return true;
}

CT_AbstractPointCloudIndex *CT_AbstractFilter_XYZ::inputPointCloudIndex() const
{
    return _inCloud;
}

CT_PointCloudIndexVector *CT_AbstractFilter_XYZ::outputPointCloudIndex() const
{
    return _outCloud;
}


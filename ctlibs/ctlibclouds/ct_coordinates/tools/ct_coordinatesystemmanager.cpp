#include "ct_coordinatesystemmanager.h"

#include "ct_global/ct_cloudscontext.h"
#include "ct_global/ct_repositorymanager.h"
#include "ct_global/ct_repository.h"
#include "ct_coordinates/ct_defaultcoordinatesystem.h"
#include "ct_iterator/ct_pointiterator.h"
#include "ct_cloud/tools/abstract/ct_abstractglobalcloudmanagert.h"

CT_CoordinateSystemManager::CT_CoordinateSystemManager()
{
    // create a new cloud that contains index of coordinate system for all points. It will be synchronized with the size of the global point cloud
    m_indexOfCoordinateSystemOfPoints = PS_REPOSITORY->createNewCloudT< CT_StdCloudRegisteredT< CT_CoordinateSystemCloudIndex >, CT_CoordinateSystemCloudIndex >(CT_Repository::SyncWithPointCloud);

    m_sizeOfIntTabInByte = sizeof(int)*3;

    setPrecision(1);
    init();

    PS_REPOSITORY->globalCloudManager<CT_PointData>()->addGlobalCloudListener(this);
}

CT_CoordinateSystemManager::~CT_CoordinateSystemManager()
{
    clear();
}

bool CT_CoordinateSystemManager::setPrecision(const double &precision)
{
    if(m_pAccess.size() != 0)
        return false;

    m_csKeySum = (1020*precision);
    //m_csKeySum = (1677.7216*precision);
    //m_csKeySum = (10*precision);
    m_csKeyDividor = m_csKeySum*2;

    return true;
}

CT_AbstractCoordinateSystem* CT_CoordinateSystemManager::computeCoordinateSystemForPointAndAddItToCollection(const CT_Point &p, GLuint &csIndex)
{
    int tab[3];

    uint key = computeUniqueKeyForCoordinatesAndFillTab(p(CT_Point::X), p(CT_Point::Y), p(CT_Point::Z), tab);

    if(m_lastCsKeyUsed == key) {

        csIndex = m_lastCsIndexUsed;

        return m_lastCsUsed;
    }

    m_lastCsKeyUsed = key;

    int i = 0;

    QVector<uint>::const_iterator begin = m_csKey.begin();
    QVector<uint>::const_iterator end = m_csKey.end();

    while(begin != end) {

        if((*begin) == key) {
            m_lastCsUsed = m_cs[i];
            m_lastCsIndexUsed = i;

            csIndex = m_lastCsIndexUsed;

            return m_lastCsUsed;
        }

        ++begin;
        ++i;
    }

    CT_DefaultCoordinateSystem *coordinateSystem = new CT_DefaultCoordinateSystem(tab[0]*m_csKeyDividor,
                                                                                  tab[1]*m_csKeyDividor,
                                                                                  tab[2]*m_csKeyDividor);

    coordinateSystem->setUniqueKey(key);

    m_cs.push_back(coordinateSystem);
    m_csKey.push_back(key);

    m_lastCsUsed = coordinateSystem;
    m_lastCsIndexUsed = i;

    csIndex = m_lastCsIndexUsed;

    return coordinateSystem;
}

CT_AbstractCoordinateSystem *CT_CoordinateSystemManager::createCoordinateSystemForCoordinates(const double &x, const double &y, const double &z) const
{
    int tab[3];

    uint key = computeUniqueKeyForCoordinatesAndFillTab(x, y, z, tab);

    CT_DefaultCoordinateSystem *s =  new CT_DefaultCoordinateSystem(tab[0]*m_csKeyDividor,
                                                                    tab[1]*m_csKeyDividor,
                                                                    tab[2]*m_csKeyDividor);
    s->setUniqueKey(key);

    return s;
}

uint CT_CoordinateSystemManager::computeUniqueKeyForCoordinates(const double &x, const double &y, const double &z) const
{
    int tab[3];
    return computeUniqueKeyForCoordinatesAndFillTab(x, y, z, tab);
}

CT_AbstractCoordinateSystem* CT_CoordinateSystemManager::coordinateSystemForPointAt(const size_t &globalIndex) const
{
    return coordinateSystemAt(m_indexOfCoordinateSystemOfPoints->cloudT()->valueAt(globalIndex));
}

GLuint CT_CoordinateSystemManager::coordinateSystemIndexForPointAt(const size_t &globalIndex) const
{
    return m_indexOfCoordinateSystemOfPoints->cloudT()->valueAt(globalIndex);
}

void CT_CoordinateSystemManager::setCoordinateSystemForPointAt(const size_t &globalIndex, const GLuint &coordinateSystemIndex) const
{
    m_indexOfCoordinateSystemOfPoints->cloudT()->replaceValueAt(globalIndex, coordinateSystemIndex);
}

int CT_CoordinateSystemManager::size() const
{
    return m_cs.size();
}

CT_AbstractCoordinateSystem* CT_CoordinateSystemManager::coordinateSystemAt(const GLuint &csIndex) const
{
    return m_cs.at(csIndex);
}

GLuint CT_CoordinateSystemManager::indexOfCoordinateSystem(const CT_AbstractCoordinateSystem *cs) const
{
    return m_cs.indexOf((CT_AbstractCoordinateSystem*)cs);
}

void CT_CoordinateSystemManager::clear()
{
    qDeleteAll(m_cs.begin(), m_cs.end());

    m_cs.resize(0);
    m_csKey.resize(0);
}

uint CT_CoordinateSystemManager::computeUniqueKeyForCoordinatesAndFillTab(const double &x, const double &y, const double &z, int tab[]) const
{
    if(x > 0)
        tab[0] = (x+m_csKeySum)/m_csKeyDividor;
    else
        tab[0] = (x-m_csKeySum)/m_csKeyDividor;

    if(y > 0)
        tab[1] = (y+m_csKeySum)/m_csKeyDividor;
    else
        tab[1] = (y-m_csKeySum)/m_csKeyDividor;

    if(z > 0)
        tab[2] = (z+m_csKeySum)/m_csKeyDividor;
    else
        tab[2] = (z-m_csKeySum)/m_csKeyDividor;

    return qHashBits(&tab[0], 3 * sizeof(int), 0);
}

void CT_CoordinateSystemManager::computeOffsetForTab(int tab[], Eigen::Vector3d &offset) const
{
    offset[0] = tab[0]*m_csKeyDividor;
    offset[1] = tab[1]*m_csKeyDividor;
    offset[2] = tab[2]*m_csKeyDividor;
}

CT_CoordinateSystemCloudIndex* CT_CoordinateSystemManager::indexCloudOfCoordinateSystemOfPoints() const
{
    return m_indexOfCoordinateSystemOfPoints->cloudT();
}

void CT_CoordinateSystemManager::init()
{
    clear();

    CT_DefaultCoordinateSystem *cs = new CT_DefaultCoordinateSystem(0, 0, 0);

    m_lastCsUsed = cs;
    m_lastCsKeyUsed = 0;
    m_lastCsIndexUsed = 0;

    m_cs.push_back(m_lastCsUsed);
    m_csKey.push_back(m_lastCsKeyUsed);
}

void CT_CoordinateSystemManager::cloudDeleted(const size_t &beginIndex, const size_t &size)
{
    Q_UNUSED(beginIndex)
    Q_UNUSED(size)

    const size_t s = m_pAccess.size();

    if(s == 0)
        init();
}

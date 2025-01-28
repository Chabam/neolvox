#include "ct_globalpointcloudmanager.h"

#include "ct_pointcloudindex/ct_pointcloudindexvector.h"
#include "ct_pointcloudindex/ct_pointcloudindexlessmemory.h"
#include "ct_iterator/ct_pointiterator.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"

CT_GlobalPointCloudManager::CT_GlobalPointCloudManager() : QObject(), CT_GlobalCloudManagerT<CT_PointData, CT_InternalPointCloud>()
{
    m_uspc.first = nullptr;
}

CT_GlobalPointCloudManager::~CT_GlobalPointCloudManager()
{
    delete m_uspc.first;
}

CT_AbstractUndefinedSizePointCloud* CT_GlobalPointCloudManager::createNewUndefinedSizePointCloud()
{
    if(m_uspc.first == nullptr)
    {
        m_uspc.first = new CT_StandardUndefinedSizePointCloud(m_cloud.size(), &m_cloud);
        m_uspc.second = m_uspc.first->beginIndex();

        connect(m_uspc.first, SIGNAL(destroyed(QObject*)), this, SLOT(undefinedSizePointCloudDestroyed(QObject*)), Qt::DirectConnection);

        return m_uspc.first;
    }

    return nullptr;
}

CT_NMPCIR CT_GlobalPointCloudManager::registerUndefinedSizePointCloud(CT_AbstractUndefinedSizePointCloud *uspc, IndexOptimization optim)
{
    CT_NMPCIR cir;

    if(uspc == m_uspc.first)
    {
        disconnect(m_uspc.first, nullptr, this, nullptr);

        size_t size = m_uspc.first->size();

        if(size > 0)
            cir = createCIR<CT_PointCloudIndexLessMemory, CT_PointCloudIndexVector>(m_uspc.first->beginIndex(), size, optim);

        const std::vector<GLuint> &csIndexes = m_uspc.first->coordinateSystemIndexOfPointsAdded();

        size_t i = 0;
        CT_PointIterator it(cir);

        while(it.hasNext()) {
            PS_COORDINATES_SYS_MANAGER->setCoordinateSystemForPointAt(it.next().cIndex(), csIndexes[i]);
            ++i;
        }

        delete m_uspc.first;
        m_uspc.first = nullptr;
    }

    return cir;
}

void CT_GlobalPointCloudManager::undefinedSizePointCloudDestroyed(QObject *o)
{
    Q_UNUSED(o)

    if(m_uspc.second < m_cloud.size())
    {
        resizeCloud(m_uspc.second);
    }
}

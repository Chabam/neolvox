#include "ct_repository.h"

#include "ct_cloud/tools/ct_cloudsynctoglobalcloudmanager.h"
#include "ct_cloudindex/tools/ct_cloudindexregistrationmanagert.h"

#include "ct_colorcloud/registered/ct_standardcolorcloudregistered.h"
#include "ct_colorcloud/ct_colorcloudstdvector.h"
#include "ct_colorcloud/ct_indexcloudcolorstdmapt.h"

#include "ct_normalcloud/registered/ct_standardnormalcloudregistered.h"
#include "ct_normalcloud/ct_normalcloudstdvector.h"

#include "ct_cloudindex/ct_cloudindexstdlistt.h"

#include "ct_cloud/tools/ct_globalpointcloudmanager.h"
#include "ct_cloud/tools/ct_globaledgecloudmanager.h"
#include "ct_cloud/tools/ct_globalfacecloudmanager.h"

#include "ct_mesh/ct_face.h"
#include "ct_mesh/ct_edge.h"

#include <QMutexLocker>

CT_NMPCIR CT_Repository::createNewPointCloud(const size_t &size, CloudIndexOptimizationType optim)
{
    return createNewCloud<CT_PointData>(size, optim);
}

CT_AbstractUndefinedSizePointCloud* CT_Repository::createNewUndefinedSizePointCloud()
{
    QMutexLocker locker(&m_mutexUSPC);

    return m_gpcManager->createNewUndefinedSizePointCloud();
}

CT_CCR CT_Repository::createNewColorCloud(SyncCloudWith syncWith)
{
    return createNewCloudT<CT_StandardColorCloudRegistered, CT_ColorCloudStdVector>(syncWith);
}

CT_NCR CT_Repository::createNewNormalCloud(SyncCloudWith syncWith)
{
    return createNewCloudT<CT_StandardNormalCloudRegistered, CT_NormalCloudStdVector>(syncWith);
}

CT_MCIR CT_Repository::createNewIndexCloud(CT_Repository::SyncCloudWith syncWith)
{
    return createNewIndexCloudT< CT_CloudIndexStdListT >(syncWith);
}

CT_MCIR CT_Repository::createNewMapIndexCloudColor(CT_Repository::SyncCloudWith syncWith)
{
    return createNewIndexCloudT< CT_IndexCloudColorStdMapT >(syncWith);
}

CT_NMPCIR CT_Repository::registerUndefinedSizePointCloud(CT_AbstractUndefinedSizePointCloud *uspc, CT_Repository::CloudIndexOptimizationType optim)
{
    QMutexLocker locker(&m_mutexUSPC);

    return m_gpcManager->registerUndefinedSizePointCloud(uspc, CT_GlobalPointCloudManager::IndexOptimization(optim));
}

CT_NMPCIR CT_Repository::copyPointCloud(CT_PCIR pcir, CloudIndexOptimizationType optim)
{
    return m_gpcManager->copyCloud<CT_PointCloudIndexLessMemory, CT_PointCloudIndexVector>(pcir, CT_GlobalPointCloudManager::IndexOptimization(optim));
}

CT_NMPCIR CT_Repository::copyPointCloud(const CT_AbstractPointCloudIndex *index, CT_Repository::CloudIndexOptimizationType optim)
{
    return m_gpcManager->copyCloud<CT_PointCloudIndexLessMemory, CT_PointCloudIndexVector>(index, CT_GlobalPointCloudManager::IndexOptimization(optim));
}

CT_NMPCIR CT_Repository::mergePointCloudContiguous(const QList< CT_PCIR > &pcir_collection)
{
    return m_pcirManager->mergeCloudContiguous(pcir_collection);
}

CT_MPCIR CT_Repository::registerPointCloudIndex(CT_AbstractModifiablePointCloudIndex *index)
{
    return registerCloudIndex<CT_PointData>(index);
}

CT_MFCIR CT_Repository::registerFaceCloudIndex(CT_AbstractModifiableFaceCloudIndex *index)
{
    return registerCloudIndex<CT_Face>(index);
}

CT_MECIR CT_Repository::registerEdgeCloudIndex(CT_AbstractModifiableEdgeCloudIndex *index)
{
    return registerCloudIndex<CT_Edge>(index);
}

const CT_InternalPointCloud* CT_Repository::internalConstPointCloud() const
{
    return m_gpcManager->globalCloud();
}

CT_AbstractPointCloud* CT_Repository::globalPointCloud() const
{
    return m_gpcManager->globalCloud();
}

CT_AbstractEdgeCloud* CT_Repository::globalEdgeCloud() const
{
    return m_gecManager->globalCloud();
}

CT_AbstractFaceCloud* CT_Repository::globalFaceCloud() const
{
    return m_gfcManager->globalCloud();
}

void CT_Repository::begingDeleteMultiCloud()
{
    m_gpcManager->beginMultiDelete();
    m_gecManager->beginMultiDelete();
    m_gfcManager->beginMultiDelete();
}

void CT_Repository::endDeleteMultiCloud()
{
    // first we delete faces
    m_gfcManager->endMultiDelete();
    // then edges
    m_gecManager->endMultiDelete();
    // and finally points
    m_gpcManager->endMultiDelete();
}

// PROTECTED //

CT_Repository::CT_Repository()
{
    m_gpcManager = new CT_GlobalPointCloudManager();
    m_gecManager = new CT_GlobalEdgeCloudManager();
    m_gecManager->setGlobalCloudManager(m_gpcManager);
    m_gfcManager = new CT_GlobalFaceCloudManager(m_gecManager);
    m_gecManager->setGlobalCloudManager(m_gfcManager);

    m_pcirManager = new CT_PointCloudIndexRegistrationManager(*m_gpcManager);
    m_fcirManager = new CT_FaceCloudIndexRegistrationManager(*m_gfcManager);
    m_ecirManager = new CT_EdgeCloudIndexRegistrationManager(*m_gecManager);

    m_syncPointCloudManager = new CT_SyncPointCloudManager(*m_gpcManager);
    m_syncFaceCloudManager = new CT_SyncFaceCloudManager(*m_gfcManager);
    m_syncEdgeCloudManager = new CT_SyncEdgeCloudManager(*m_gecManager);
}

CT_Repository::~CT_Repository()
{
    delete m_pcirManager;
    delete m_fcirManager;
    delete m_ecirManager;

    delete m_syncPointCloudManager;
    delete m_syncFaceCloudManager;
    delete m_syncEdgeCloudManager;

    delete m_gfcManager;
    delete m_gecManager;
    delete m_gpcManager;
}

// TEMPLATE //

template<>
CT_AbstractPointCloud* CT_Repository::globalCloud() const
{
    return m_gpcManager->globalCloud();
}

template<>
CT_AbstractFaceCloud* CT_Repository::globalCloud() const
{
    return m_gfcManager->globalCloud();
}

template<>
CT_AbstractEdgeCloud* CT_Repository::globalCloud() const
{
    return m_gecManager->globalCloud();
}

template<>
CT_AbstractGlobalPointCloudManager* CT_Repository::globalCloudManager() const
{
    return m_gpcManager;
}

template<>
CT_AbstractGlobalFaceCloudManager* CT_Repository::globalCloudManager() const
{
    return m_gfcManager;
}

template<>
CT_AbstractGlobalEdgeCloudManager* CT_Repository::globalCloudManager() const
{
    return m_gecManager;
}

template<>
CT_NMPCIR CT_Repository::resizeCloudIndexAndGlobalCloud(CT_NMPCIR cir, const size_t &newSize)
{
    return m_gpcManager->resizeCloudAndCloudIndex(cir, newSize);
}

template<>
CT_NMFCIR CT_Repository::resizeCloudIndexAndGlobalCloud(CT_NMFCIR cir, const size_t &newSize)
{
    return m_gfcManager->resizeCloudAndCloudIndex(cir, newSize);
}

template<>
CT_NMECIR CT_Repository::resizeCloudIndexAndGlobalCloud(CT_NMECIR cir, const size_t &newSize)
{
    return m_gecManager->resizeCloudAndCloudIndex(cir, newSize);
}

template<>
CT_NMPCIR CT_Repository::createNewCloud(const size_t &size, CT_Repository::CloudIndexOptimizationType optim)
{
    return m_gpcManager->createNewCloud<CT_PointCloudIndexLessMemory, CT_PointCloudIndexVector>(size, CT_GlobalPointCloudManager::IndexOptimization(optim));
}

template<>
CT_NMFCIR CT_Repository::createNewCloud(const size_t &size, CT_Repository::CloudIndexOptimizationType optim)
{
    return m_gfcManager->createNewCloud< CT_FaceCloudIndexLessMemory , CT_FaceCloudIndexVector >(size, CT_AbstractGlobalCloudManagerT<CT_Face>::IndexOptimization(optim));
}

template<>
CT_NMECIR CT_Repository::createNewCloud(const size_t &size, CT_Repository::CloudIndexOptimizationType optim)
{
    return m_gecManager->createNewCloud< CT_EdgeCloudIndexLessMemory , CT_EdgeCloudIndexVector >(size, CT_AbstractGlobalCloudManagerT<CT_Edge>::IndexOptimization(optim));
}

template<>
CT_MPCIR CT_Repository::registerCloudIndex(CT_AbstractModifiablePointCloudIndex *index)
{
    return m_pcirManager->registerIndex(index);
}

template<>
CT_MFCIR CT_Repository::registerCloudIndex(CT_AbstractModifiableFaceCloudIndex *index)
{
    return m_fcirManager->registerIndex(index);
}

template<>
CT_MECIR CT_Repository::registerCloudIndex(CT_AbstractModifiableEdgeCloudIndex *index)
{
    return m_ecirManager->registerIndex(index);
}

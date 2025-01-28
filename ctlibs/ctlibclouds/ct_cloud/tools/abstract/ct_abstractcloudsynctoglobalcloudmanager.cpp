#include "ct_abstractcloudsynctoglobalcloudmanager.h"

#include "ct_global/ct_cloudscontext.h"

CT_AbstractCloudSyncToGlobalCloudManager::CT_AbstractCloudSyncToGlobalCloudManager(const CT_AbstractGlobalCloudManager &gcManager)
{
    m_syncProgress = 0;
    m_gcManager = (CT_AbstractGlobalCloudManager*)&gcManager;
    m_gcManager->addGlobalCloudListener(this);
}

CT_AbstractCloudSyncToGlobalCloudManager::~CT_AbstractCloudSyncToGlobalCloudManager()
{
    while(!m_crArray.isEmpty())
        m_crArray.takeLast()->setSyncCloudManager(nullptr);
}

CT_AbstractGlobalCloudManager* CT_AbstractCloudSyncToGlobalCloudManager::globalCloudManager() const
{
    return m_gcManager;
}

QList<CT_AbstractCloudRegistered*> CT_AbstractCloudSyncToGlobalCloudManager::cloudsRegistered() const
{
    return m_crArray;
}

void CT_AbstractCloudSyncToGlobalCloudManager::setSyncProgress(int progress)
{
    if(progress != m_syncProgress)
    {
        m_syncProgress = progress;
        emit syncProgressChanged(progress);
    }
}

void CT_AbstractCloudSyncToGlobalCloudManager::cloudDeleted(const size_t &beginIndex, const size_t &size)
{
    QMutexLocker locker(&m_mutex);
    emit beginSync();

    setSyncProgress(0);

    int iSync = 0;
    float syncDividor = 1;

    if(!m_crArray.isEmpty())
        syncDividor = 100.0/m_crArray.size();

    QListIterator<CT_AbstractCloudRegistered*> it(m_crArray);

    while(it.hasNext())
    {
        CT_AbstractCloudRegistered *cr = it.next();

        cr->abstractCloud()->erase(beginIndex, size);

        ++iSync;
        setSyncProgress(iSync*syncDividor);
    }

    setSyncProgress(100);

    emit endSync();
}

void CT_AbstractCloudSyncToGlobalCloudManager::cloudAdded(const size_t &size)
{
    QMutexLocker locker(&m_mutex);
    emit beginSync();

    setSyncProgress(0);

    int iSync = 0;
    float syncDividor = 1;

    if(!m_crArray.isEmpty())
        syncDividor = 100.0/m_crArray.size();

    QListIterator<CT_AbstractCloudRegistered*> it(m_crArray);

    while(it.hasNext())
    {
        CT_AbstractCloudRegistered *cr = it.next();

        cr->abstractCloud()->resize(cr->abstractCloud()->size() + size);

        ++iSync;
        setSyncProgress(iSync*syncDividor);
    }

    setSyncProgress(100);

    emit endSync();
}

void CT_AbstractCloudSyncToGlobalCloudManager::internalRegisterCloud(CT_AbstractCloudRegistered *cr)
{
    cr->setSyncCloudManager(this);
    m_crArray.append(cr);
}

bool CT_AbstractCloudSyncToGlobalCloudManager::internalUnregisterCloud(const CT_AbstractCloudRegistered *cr)
{
    QMutexLocker locker(&m_mutex);

    return m_crArray.removeOne((CT_AbstractCloudRegistered*)cr);
}

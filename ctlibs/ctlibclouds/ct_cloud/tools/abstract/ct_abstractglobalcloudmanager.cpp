#include "ct_cloud/tools/abstract/ct_abstractglobalcloudmanager.h"

CT_AbstractGlobalCloudManager::CT_AbstractGlobalCloudManager()
{
}

void CT_AbstractGlobalCloudManager::addGlobalCloudListener(const IGlobalCloudListener *listener)
{
    m_listener.append((IGlobalCloudListener*)listener);
}

void CT_AbstractGlobalCloudManager::informThatCloudAdded(const size_t &size)
{
    QListIterator< IGlobalCloudListener* > it(m_listener);

    while(it.hasNext())
        it.next()->cloudAdded(size);
}

void CT_AbstractGlobalCloudManager::informThatCloudWillBeDeleted(const size_t &begin, const size_t &size)
{
    QListIterator< IGlobalCloudListener* > it(m_listener);

    while(it.hasNext())
        it.next()->cloudWillBeDeleted(begin, size);
}

void CT_AbstractGlobalCloudManager::informThatCloudIndexShiftedAfterThisCloud(const size_t &begin, const size_t &size)
{
    QListIterator< IGlobalCloudListener* > it(m_listener);

    while(it.hasNext())
        it.next()->cloudIndexShiftedAfterThisCloud(begin, size);
}

void CT_AbstractGlobalCloudManager::informThatCloudDeleted(const size_t &begin, const size_t &size)
{
    QListIterator< IGlobalCloudListener* > it(m_listener);

    while(it.hasNext())
        it.next()->cloudDeleted(begin, size);
}

#ifndef CT_ABSTRACTGLOBALCLOUDMANAGER_H
#define CT_ABSTRACTGLOBALCLOUDMANAGER_H

#include "ct_cloud/abstract/ct_abstractcloud.h"
#include "ct_cloud/tools/iglobalcloudlistener.h"

#include <QList>

/**
 * Utility class who manage a global cloud. It will inform IGlobalCloudListener when elements of the cloud
 * is deleted or added. So we can sync index and other cloud with this global cloud.
 */
class CTLIBCLOUDS_EXPORT CT_AbstractGlobalCloudManager
{
public:
    CT_AbstractGlobalCloudManager();
    virtual ~CT_AbstractGlobalCloudManager() {}

    /**
     * @brief Add listener of global cloud change
     */
    void addGlobalCloudListener(const IGlobalCloudListener *listener);

    /**
     * @brief Returns the global cloud
     */
    virtual CT_AbstractCloud* globalAbstractCloud() const = 0;

protected:

    QList< IGlobalCloudListener* >  m_listener;

    void informThatCloudAdded(const size_t &size);
    void informThatCloudWillBeDeleted(const size_t &begin, const size_t &size);
    void informThatCloudIndexShiftedAfterThisCloud(const size_t &begin, const size_t &size);
    void informThatCloudDeleted(const size_t &begin, const size_t &size);
};

#endif // CT_ABSTRACTGLOBALCLOUDMANAGER_H

#ifndef CT_ABSTRACTCLOUDSYNCTOGLOBALCLOUDMANAGER_H
#define CT_ABSTRACTCLOUDSYNCTOGLOBALCLOUDMANAGER_H

#include "ct_cloud/registered/abstract/ct_abstractcloudregistered.h"
#include "ct_cloud/tools/abstract/ct_abstractglobalcloudmanager.h"

#include <QObject>
#include <QMutex>

class CTLIBCLOUDS_EXPORT CT_AbstractCloudSyncToGlobalCloudManager : public QObject, public IGlobalCloudListener
{
    Q_OBJECT

public:
    CT_AbstractCloudSyncToGlobalCloudManager(const CT_AbstractGlobalCloudManager &gcManager);
    virtual ~CT_AbstractCloudSyncToGlobalCloudManager();

    /**
     * @brief Return the global cloud manager
     */
    CT_AbstractGlobalCloudManager* globalCloudManager() const;

    /**
     * @brief Returns the list of clouds registered
     */
    QList<CT_AbstractCloudRegistered*> cloudsRegistered() const;

protected:

    friend class CT_AbstractCloudRegistered;

    void internalRegisterCloud(CT_AbstractCloudRegistered *cr);
    bool internalUnregisterCloud(const CT_AbstractCloudRegistered *cr);

    /**
     * @brief Called when the global cloud is imputed of elements between beginIndex and endIndex (endIndex = beginIndex+size).
     */
    void cloudDeleted(const size_t &beginIndex, const size_t &size);

    /**
     * @brief Called when the size of the global cloud has increased
     */
    void cloudAdded(const size_t &size);

private:

    /**
     * @brief Global cloud manager
     */
    CT_AbstractGlobalCloudManager   *m_gcManager;

    /**
     * @brief la liste des nuages enregistrés (à synchroniser)
     */
    QList<CT_AbstractCloudRegistered*>                      m_crArray;

    /**
     * @brief la valeur de progression de la synchronisation
     */
    int m_syncProgress;

    /**
     * @brief Définir la progression de la synchronisation des nuages d'index
     * @param progress : la valeur de progression
     */
    void setSyncProgress(int progress);

protected:

    /**
     * @brief A mutex
     */
    QMutex                                                  m_mutex;

signals:
    void beginSync();
    void syncProgressChanged(int progress);
    void endSync();
};

#endif // CT_ABSTRACTCLOUDSYNCTOGLOBALCLOUDMANAGER_H

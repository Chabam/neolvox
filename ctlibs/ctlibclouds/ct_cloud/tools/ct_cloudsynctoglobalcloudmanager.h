#ifndef CT_CLOUDSYNCTOGLOBALCLOUDMANAGER_H
#define CT_CLOUDSYNCTOGLOBALCLOUDMANAGER_H

#include "ct_cloud/tools/abstract/ct_abstractcloudsynctoglobalcloudmanager.h"

#include <QSharedPointer>

/**
 * Utility class who can create a cloud of T (color, normal, etc....) that will be sync with
 * the global cloud in the CT_AbstractGlobalCloudManager (per example a cloud of point or a cloud of face, etc...).
 *
 * When the cloud created is destroyed from memory it will automatically be unregistered.
 */
class CTLIBCLOUDS_EXPORT CT_CloudSyncToGlobalCloudManager : public CT_AbstractCloudSyncToGlobalCloudManager
{
public:
    CT_CloudSyncToGlobalCloudManager(const CT_AbstractGlobalCloudManager &gcManager);

    /**
     * @brief Create a new cloud whose is size while be synchronized with the global cloud's size
     *
     * @param optionnalParameterForConstructor : an optionnal parameter passed to constructor
     *
     * This method is implemented like this :
     *
     * return QSharedPointer<CloudRegistered>( new CloudRegistered( new Cloud( globalCloud->size() , (CastOptionnalParameter*)optionnalParameterForConstructor ) ) );
     *
     * @return return the cloud registered
     */
    template< class CloudRegistered, class Cloud, typename CastOptionnalParameter >
    QSharedPointer< CloudRegistered > createNewCloud(void *optionnalParameterForConstructor = nullptr)
    {
        QMutexLocker locker(&this->m_mutex);

        CloudRegistered *icr = new CloudRegistered( new Cloud(this->globalCloudManager()->globalAbstractCloud()->size(), (CastOptionnalParameter*)optionnalParameterForConstructor) );
        this->internalRegisterCloud(icr);

        return QSharedPointer< CloudRegistered >(icr);
    }

    /**
     * @brief Create a new cloud whose is size while be synchronized with the global cloud's size
     *
     * This method is implemented like this :
     *
     * return QSharedPointer<CloudRegistered>( new CloudRegistered( new Cloud( globalCloud->size() ) ) );
     *
     * @return return the cloud registered
     */
    template< class CloudRegistered, class Cloud >
    QSharedPointer< CloudRegistered > createNewCloud()
    {
        QMutexLocker locker(&this->m_mutex);

        CloudRegistered *icr = new CloudRegistered( new Cloud(this->globalCloudManager()->globalAbstractCloud()->size()) );
        this->internalRegisterCloud(icr);

        return QSharedPointer< CloudRegistered >(icr);
    }
};

#endif // CT_CLOUDSYNCTOGLOBALCLOUDMANAGER_H

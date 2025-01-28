#ifndef CT_REPOSITORY_H
#define CT_REPOSITORY_H

#include "ct_defines.h"
#include "ct_pointcloud/tools/abstract/ct_abstractundefinedsizepointcloud.h"
#include "ct_cloud/tools/ct_cloudsynctoglobalcloudmanager.h"

#include <QMutex>

class CTLIBCLOUDS_EXPORT CT_Repository
{
public:
    /**
     * @brief Type of synchronization for new cloud index
     */
    enum SyncCloudWith {
        SyncWithPointCloud, // sync cloud index with point cloud
        SyncWithFaceCloud,  // sync cloud index with face cloud
        SyncWithEdgeCloud   // sync cloud index with edge cloud
    };

    /**
     * @brief Type of optimization in memory for new cloud index
     */
    enum CloudIndexOptimizationType
    {
        MemoryOptimized = 0,    // Add only a "begin index" and a "size" variables in the class that manage index.
        AccessSpeedOptimized    // Add one index for one points.
    };

    ~CT_Repository();

    /**
     * @brief Create a new point cloud in memory (Update the size of the global points cloud).
     *
     *        When you delete from memory the object returned by this method, the points cloud will be automatically
     *        removed from global points cloud and its size will be updated. The index manager will automatically update
     *        all cloud index that points to elements after the cloud deleted.
     *
     * @param size : the size of the point cloud to create
     * @param optim : optimization type
     * @return Returns a cloud index that points to new points created. This cloud index is not modifiable but points are.
     */
    CT_NMPCIR createNewPointCloud(const size_t &size, CloudIndexOptimizationType optim = MemoryOptimized);

    /**
     * @brief Create a new point cloud in memory that size is not known.
     * @warning You can call once again this method only after you have registered the object returned
     *          by this method : use the method "registerUndefinedSizePointCloud"
     *
     * @return Returns nullptr if a previous CT_AbstractUndefinedSizePointCloud was created but not registered. Otherwise returns an object
     *         that can be used to add/remove new points to global points cloud.
     */
    CT_AbstractUndefinedSizePointCloud* createNewUndefinedSizePointCloud();

    /**
     * @brief Create a new colors cloud whose is size is synchronized with the global XXX cloud. This means that the size
     *        of the cloud index will always be sync with the global cloud and indices will be automatically modified if
     *        a cloud is deleted.
     *
     * @param syncWith : define what cloud sync
     * @return return the colors cloud registered
     */
    CT_CCR createNewColorCloud(SyncCloudWith syncWith);

    /**
     * @brief Create a new normals cloud whose is size is synchronized with the global XXX cloud. This means that the size
     *        of the cloud index will always be sync with the global cloud and indices will be automatically modified if
     *        a cloud is deleted.
     *
     * @param syncWith : define what cloud sync
     * @return return the normals cloud registered
     */
    CT_NCR createNewNormalCloud(SyncCloudWith syncWith);

    /**
     * @brief Create a new cloud of type CloudT that will be synchronized with the global XXX cloud (defined by syncWith param). This
     *        means that the size of the cloud index will always be sync with the global cloud and indices will be automatically modified if
     *        a cloud is deleted.
     *
     * @param syncWith : define what cloud sync
     * @example : create a cloud of quint8 that is synchronized with the global points cloud :
     *
     *            QSharedPointer< CT_StdCloudRegisteredT< CT_StandardCloudStdVectorT<quint8> > > sp = createNewCloudT< CT_StdCloudRegisteredT< CT_StandardCloudStdVectorT<quint8> >, CT_StandardCloudStdVectorT<quint8> >(CT_Repository::SyncWithPointCloud);
     *
     * @return return the CloudT cloud registered
     */
    template<class CloudRegistered, class Cloud >
    QSharedPointer< CloudRegistered > createNewCloudT(SyncCloudWith syncWith)
    {
        if(syncWith == SyncWithPointCloud)
            return m_syncPointCloudManager->createNewCloud<CloudRegistered, Cloud >();
        else if(syncWith == SyncWithFaceCloud)
            return m_syncFaceCloudManager->createNewCloud<CloudRegistered, Cloud >();
        else if(syncWith == SyncWithEdgeCloud)
            return m_syncEdgeCloudManager->createNewCloud<CloudRegistered, Cloud >();

        return QSharedPointer< CloudRegistered >(nullptr);
    }

    /**
     * @brief Create a new index cloud (of type CT_CloudIndexStdListT) whose indices is synchronized with index of cloud passed in parameter. This means that
     *        indices will be automatically modified if a cloud is deleted.
     *
     * @param syncWith : define what cloud sync
     * @return return the index cloud registered
     */
    CT_MCIR createNewIndexCloud(SyncCloudWith syncWith);

    /**
     * @brief Create a new index cloud whose indices is synchronized with index of cloud passed in parameter. This means that
     *        indices will be automatically modified if a cloud is deleted. The cloud is a map : QMap<size_t, Color>, key is indices
     *        and value the color.
     *
     * @example An example of how to use the result of this method :
     *
     *        CT_MCIR mccr = createNewMapIndexCloudColor(CT_Repository::SyncWithXXXCloud);
     *        CT_AbstractModifiableIndexCloudColorMap *map = dynamic_cast<CT_AbstractModifiableIndexCloudColorMap*>(mccr->abstractModifiableCloudIndex());
     *        map->insertIndexAndColor(0, color);
     *
     * @return return the index cloud registered
     */
    CT_MCIR createNewMapIndexCloudColor(SyncCloudWith syncWith);

    /**
     * @brief Create a new index cloud of type IndexCloudT that will be synchronized with the global XXX cloud (defined by syncWith param). This
     *        means that indices of the cloud index will always be sync with the global cloud.
     *
     * @param syncWith : define what cloud sync
     * @example : create a CT_CloudIndexStdVectorT that is synchronized with the global points cloud :
     *
     *            CT_MCIR mcir = createNewIndexCloudT< CT_CloudIndexStdVectorT >(CT_Repository::SyncWithPointCloud);
     *
     * @return return the index cloud registered
     */
    template< template<typename T> class IndexCloudT >
    CT_MCIR createNewIndexCloudT(SyncCloudWith syncWith)
    {
        if(syncWith == SyncWithPointCloud)
            return registerCloudIndex<CT_PointData>(new IndexCloudT<CT_PointData>());
        else if(syncWith == SyncWithFaceCloud)
            return registerCloudIndex<CT_Face>(new IndexCloudT<CT_Face>());
        else if(syncWith == SyncWithEdgeCloud)
            return registerCloudIndex<CT_Edge>(new IndexCloudT<CT_Edge>());

        return CT_MCIR(nullptr);
    }

    /**
     * @brief Create a new index cloud of type IndexCloudT that will be synchronized with the global XXX cloud (defined by PEF template). This
     *        means that indices of the cloud index will always be sync with the global cloud.
     *
     * @param syncWith : define what cloud sync
     * @example : --- Create a CT_CloudIndexStdVectorT that is synchronized with the global points cloud :
     *
     *            CT_MCIR mcir = createNewIndexCloudT< CT_PointData, CT_CloudIndexStdVectorT >();
     *
     *            or
     *
     *            CT_MCIR mcir = createNewIndexCloudT< CT_Point, CT_CloudIndexStdVectorT >();
     *
     *            --- Create a CT_IndexCloudColorStdMapT that is synchronized with the global edge cloud :
     *
     *            CT_MCIR mcir = createNewIndexCloudT< CT_Edge, CT_IndexCloudColorStdMapT >();
     *
     * @return return the index cloud registered
     */
    template< typename PEF, template<typename T> class IndexCloudT >
    CT_MCIR createNewIndexCloudTSyncWithPEF()
    {
        const PEF pef;
        return internalCreateNewIndexCloudTSyncWithPEF<IndexCloudT>(pef);
    }

    /**
     * @brief Register the object create with the method "createNewUndefinedSizePointCloud". Convert the points added to a
     *        point cloud index.
     *
     * @warning the object passed in parameter will be deleted from memory after you call this method. You can not use it.
     * @param uspc : the object created by the method "createNewUndefinedSizePointCloud
     * @param optim : optimization type
     * @return Returns a cloud index that points to new points created. This cloud index is not modifiable but points are.
     */
    CT_NMPCIR registerUndefinedSizePointCloud(CT_AbstractUndefinedSizePointCloud *uspc, CloudIndexOptimizationType optim = MemoryOptimized);

    /**
     * @brief Recopie EN MEMOIRE les points du nuage de points défini par le nuage d'index contenu dans le nuage de points enregistré passé en paramètre.
     *
     *        Crée un nouvel espace dans le nuage de points global de la taille du nuage d'index. Copie
     *        les points défini par le nuage d'index dans ce nouvel espace. Crée un nouveau nuage d'index non modifiable et l'enregistre auprès
     *        du gestionnaire d'index.
     *
     * @param pcr : le nuage de points enregistré à copier
     * @param optim : le type d'optimisation du nuage d'index
     * @return Retourne un nuage d'index correspondant à l'emplacement en mémoire des nouveaux points. Ce nuage d'index n'est pas modifiable !
     */
    CT_NMPCIR copyPointCloud(CT_PCIR pcir, CloudIndexOptimizationType optim = MemoryOptimized);

    /**
     * @brief Recopie EN MEMOIRE les points du nuage de points défini par le nuage d'index.
     *
     *        Crée un nouvel espace dans le nuage de points global de la taille du nuage d'index. Copie
     *        les points défini par le nuage d'index dans ce nouvel espace. Copie le nuage d'index et l'enregistre auprès
     *        du gestionnaire d'index.
     *
     * @param pcr : le nuage de points enregistré à copier
     * @param optim : le type d'optimisation du nuage d'index
     * @return Retourne un nuage d'index correspondant à l'emplacement en mémoire des nouveaux points. Ce nuage d'index n'est pas modifiable !
     */
    CT_NMPCIR copyPointCloud(const CT_AbstractPointCloudIndex *index, CloudIndexOptimizationType optim = MemoryOptimized);

    /**
     * @brief Return a memory optimized points cloud index that begin at the first index of the first points cloud and finish at the end of the last points cloud of the list
     * @param pcir_collection : the list of points cloud that is contiguous in memory
     * @return A memory optimized points cloud index. This points cloud index is not modifiable !
     */
    CT_NMPCIR mergePointCloudContiguous(const QList< CT_PCIR > &pcir_collection);

    /**
     * @brief Enregistre le nuage d'index auprès du gestionnaire. Le gestionnaire va modifier (décaler) les index du nuage si un nuage de points
     *        est supprimé de la mémoire et que vos index deviennent invalide.
     *
     *        Si vos index correspondent à un nuage de points qui a été supprimé les index seront supprimés. Si vos index correspondent à un nuage
     *        de points qui se trouve après celui qui a été supprimé, vos index seront décalés. Si certains index correspondent à un nuage de points qui
     *        à été supprimé ils seront par la même occasion supprimé.
     *
     *        Ainsi les index sont toujours synchronisés par rapport au nuage de points global.
     *
     *       PROTEGE PAR UN MUTEX
     *
     * @param index : le nuage d'index que vous avez créé. (il n'a pas besoin d'être remplis)
     * @return Retourne un objet contenant le nuage de points (global) et le nuage d'index du nuage de points.
     */
    CT_MPCIR registerPointCloudIndex(CT_AbstractModifiablePointCloudIndex *index);
    CT_MFCIR registerFaceCloudIndex(CT_AbstractModifiableFaceCloudIndex *index);
    CT_MECIR registerEdgeCloudIndex(CT_AbstractModifiableEdgeCloudIndex *index);

    /**
     * @brief Returns the internal point cloud (use this method to pass it to opengl engine)
     */
    const CT_InternalPointCloud* internalConstPointCloud() const;

    /**
     * @brief Call this method when you begin to delete multiple cloud (to accelerate the process)
     *
     *        Called from CT_ResultGroup
     */
    void begingDeleteMultiCloud();

    /**
     * @brief Call this method when you have finished to delete multiple cloud
     *
     *        Called from CT_ResultGroup
     */
    void endDeleteMultiCloud();

    /**
     * @brief Returns the global point cloud (contains all points created)
     */
    CT_AbstractPointCloud* globalPointCloud() const;

    /**
     * @brief Returns the global edge cloud (contains all edges created)
     */
    CT_AbstractEdgeCloud* globalEdgeCloud() const;

    /**
     * @brief Returns the global face cloud (contains all faces created)
     */
    CT_AbstractFaceCloud* globalFaceCloud() const;

    /**
     * @brief Returns the global T cloud (contains all T (points/faces or edges) created)
     */
    template<typename T>
    CT_AbstractCloudT<T>* globalCloud() const { return nullptr; }

protected:

    friend class CT_RepositoryManager;
    friend class CT_Mesh;
    friend class CT_AbstractUndefinedSizePointCloud;
    friend class CT_PointAccessor;
    friend class CT_FaceAccessor;
    friend class CT_EdgeAccessor;
    friend class CT_Face;
    friend class CT_Edge;
    friend class CT_CoordinateSystemManager;
    friend class CT_RepositoryAccess;
    template<typename T, typename CLOUD> friend class CT_GlobalCloudManagerT;
    template<typename T> friend class CT_MeshAllocatorT;
    template<typename T> friend class CT_CloudIndexIteratorT;

    CT_Repository();

    template<typename T>
    CT_AbstractGlobalCloudManagerT<T>* globalCloudManager() const { return nullptr; }

    /**
     * @brief IF the cloud index passed in parameter was created by the global cloud manager and IF it was at
     *        the end of the global cloud => Resize the cloud index to the 'newSize' and resize the cloud
     *        to (current size + (newSize - cloudIndex->size()) ).
     *
     *        If all condition it's ok, the cloud in the QSharedPointer is resized to the 'newSize' and the global cloud is resized to
     *        (current size + (newSize - cloudIndex->size()) )
     *
     * @return If all condition it's ok the QSharePointer passed in parameter is returned, otherwise a QSharedPointer with data() == nullptr.
     */
    template<typename T>
    QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<T> > resizeCloudIndexAndGlobalCloud(QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<T> > cir, const size_t &newSize);

    /**
     * @brief Crée un nouveau nuage en mémoire. Met à jour le nuage complet.
     *
     *        Lorsque vous allez supprimé de la mémoire l'objet retourné par cette méthode le nuage
     *        sera supprimé de la mémoire. Le gestionnaire d'index synchronisera les index toujours actifs.
     *
     * @param size : la taille du nouveau nuage à créer
     * @param optim : le type d'optimisation du nuage d'index
     * @return Retourne un nuage d'index correspondant à l'emplacement en mémoire des points. Ce nuage d'index n'est pas modifiable !
     */
    template<typename T>
    QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<T> > createNewCloud(const size_t &size, CloudIndexOptimizationType optim = MemoryOptimized);

    /**
     * @brief Enregistre le nuage d'index auprès du gestionnaire. Le gestionnaire va modifier (décaler) les index du nuage si un nuage
     *        est supprimé de la mémoire et que vos index deviennent invalide.
     *
     *        Si vos index correspondent à un nuage qui a été supprimé les index seront supprimés. Si vos index correspondent à un nuage
     *        qui se trouve après celui qui a été supprimé, vos index seront décalés. Si certains index correspondent à un nuage qui
     *        à été supprimé ils seront par la même occasion supprimé.
     *
     *        Ainsi les index sont toujours synchronisés par rapport au nuage global.
     *
     *       PROTEGE PAR UN MUTEX
     *
     * @param index : le nuage d'index que vous avez créé. (il n'a pas besoin d'être remplis)
     * @return Retourne un objet contenant le nuage global et le nuage d'index du nuage de points.
     */
    template<typename T>
    QSharedPointer< CT_AbstractModifiableCloudIndexRegisteredT<T> > registerCloudIndex(CT_AbstractModifiableCloudIndexT<T> *index);

    template< template<typename T> class IndexCloudT >
    CT_MCIR internalCreateNewIndexCloudTSyncWithPEF(const CT_PointData&)
    {
        return registerCloudIndex<CT_PointData>(new IndexCloudT<CT_PointData>());
    }

    template< template<typename T> class IndexCloudT >
    CT_MCIR internalCreateNewIndexCloudTSyncWithPEF(const CT_Point&)
    {
        return registerCloudIndex<CT_PointData>(new IndexCloudT<CT_PointData>());
    }

    template< template<typename T> class IndexCloudT >
    CT_MCIR internalCreateNewIndexCloudTSyncWithPEF(const CT_Face&)
    {
        return registerCloudIndex<CT_Face>(new IndexCloudT<CT_Face>());
    }

    template< template<typename T> class IndexCloudT >
    CT_MCIR internalCreateNewIndexCloudTSyncWithPEF(const CT_Edge&)
    {
        return registerCloudIndex<CT_Edge>(new IndexCloudT<CT_Edge>());
    }

private:

    CT_GlobalPointCloudManager              *m_gpcManager;
    CT_GlobalFaceCloudManager               *m_gfcManager;
    CT_GlobalEdgeCloudManager               *m_gecManager;

    CT_PointCloudIndexRegistrationManager   *m_pcirManager;
    CT_FaceCloudIndexRegistrationManager    *m_fcirManager;
    CT_EdgeCloudIndexRegistrationManager    *m_ecirManager;

    CT_SyncPointCloudManager                *m_syncPointCloudManager;
    CT_SyncFaceCloudManager                 *m_syncFaceCloudManager;
    CT_SyncEdgeCloudManager                 *m_syncEdgeCloudManager;

    QMutex                                  m_mutexUSPC;
};

template<>
CTLIBCLOUDS_EXPORT CT_AbstractPointCloud* CT_Repository::globalCloud() const;

template<>
CTLIBCLOUDS_EXPORT CT_AbstractFaceCloud* CT_Repository::globalCloud() const;

template<>
CTLIBCLOUDS_EXPORT CT_AbstractEdgeCloud* CT_Repository::globalCloud() const;

template<>
CTLIBCLOUDS_EXPORT CT_AbstractGlobalPointCloudManager* CT_Repository::globalCloudManager() const;

template<>
CTLIBCLOUDS_EXPORT CT_AbstractGlobalFaceCloudManager* CT_Repository::globalCloudManager() const;

template<>
CTLIBCLOUDS_EXPORT CT_AbstractGlobalEdgeCloudManager* CT_Repository::globalCloudManager() const;

template<>
CTLIBCLOUDS_EXPORT CT_NMPCIR CT_Repository::createNewCloud(const size_t &size, CT_Repository::CloudIndexOptimizationType optim);

template<>
CTLIBCLOUDS_EXPORT CT_NMFCIR CT_Repository::createNewCloud(const size_t &size, CT_Repository::CloudIndexOptimizationType optim);

template<>
CTLIBCLOUDS_EXPORT CT_NMECIR CT_Repository::createNewCloud(const size_t &size, CT_Repository::CloudIndexOptimizationType optim);

template<>
CTLIBCLOUDS_EXPORT CT_NMPCIR CT_Repository::resizeCloudIndexAndGlobalCloud(CT_NMPCIR cir, const size_t &newSize);

template<>
CTLIBCLOUDS_EXPORT CT_NMFCIR CT_Repository::resizeCloudIndexAndGlobalCloud(CT_NMFCIR cir, const size_t &newSize);

template<>
CTLIBCLOUDS_EXPORT CT_NMECIR CT_Repository::resizeCloudIndexAndGlobalCloud(CT_NMECIR cir, const size_t &newSize);

template<>
CTLIBCLOUDS_EXPORT CT_MPCIR CT_Repository::registerCloudIndex(CT_AbstractModifiablePointCloudIndex *index);

template<>
CTLIBCLOUDS_EXPORT CT_MFCIR CT_Repository::registerCloudIndex(CT_AbstractModifiableFaceCloudIndex *index);

template<>
CTLIBCLOUDS_EXPORT CT_MECIR CT_Repository::registerCloudIndex(CT_AbstractModifiableEdgeCloudIndex *index);

#endif // CT_REPOSITORY_H

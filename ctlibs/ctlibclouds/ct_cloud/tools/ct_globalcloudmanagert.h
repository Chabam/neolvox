#ifndef CT_GLOBALCLOUDMANAGERT_H
#define CT_GLOBALCLOUDMANAGERT_H

#include "ct_cloud/tools/abstract/ct_abstractglobalcloudmanagert.h"
#include "ct_pointcloud/ct_internalpointcloud.h"
#include "ct_global/ct_repositorymanager.h"
#include "ct_global/ct_repository.h"
#include "ct_cloudindex/ct_cloudindexstdvectort.h"
#include "ct_cloudindex/registered/ct_standardnotmodifiablecloudindexregisteredt.h"

struct CT_GlobalCloudIndex
{
public:
    size_t  m_begin;
    size_t  m_end;

    bool operator <(const CT_GlobalCloudIndex &b) const
    {
        return (m_end < b.m_begin);
    }
};

/**
 * Class who manage globals cloud. The manager create a CLOUD (second template) that have
 * a collection of T (first template). The CLOUD must be modifiable.
 *
 * Per example : CT_GlobalCloudManagerT<CT_PointData, CT_PointCloudIndexVector >
 *
 * It's an utility class who manage a global cloud. It will inform the CT_AbstractCloudIndexRegistrationManagerT and all
 * CT_AbstractCloudSyncToGlobalCloudManagerT when elements of the cloud is deleted or added. So we can sync index and
 * other cloud with this global cloud.
 */
template<typename T, typename CLOUD>
class CT_GlobalCloudManagerT : public CT_AbstractGlobalCloudManagerT<T>
{
public:
    typedef typename CLOUD::Iterator Iterator;
    typedef typename CLOUD::Const_Iterator Const_Iterator;

    typedef typename CT_AbstractGlobalCloudManagerT<T>::CT_AbstractModifiableCIR         CT_AbstractModifiableCIR;
    typedef typename CT_AbstractGlobalCloudManagerT<T>::CT_AbstractNotModifiableCIR      CT_AbstractNotModifiableCIR;
    typedef typename CT_AbstractGlobalCloudManagerT<T>::CT_AbstractCIR                   CT_AbstractCIR;

    CT_GlobalCloudManagerT();

    /**
     * @brief Crée un nouveau nuage en mémoire. Met à jour le nuage complet.
     *
     *        Lorsque vous allez supprimé de la mémoire l'objet retourné par cette méthode le nuage
     *        sera supprimé de la mémoire. Le gestionnaire d'index synchronisera les index toujours actifs.
     *
     * @param size : la taille du nouveau nuage à créer
     * @param optim : le type d'optimisation du nuage d'index
     * @return Retourne un nuage d'index correspondant à l'emplacement en mémoire des valeurs (T) créées. Ce nuage d'index n'est pas modifiable !
     */
    template< class IndexMemoryOptim, class IndexAccessOptim >
    CT_AbstractNotModifiableCIR createNewCloud(size_t size, typename CT_AbstractGlobalCloudManagerT<T>::IndexOptimization optim = CT_AbstractGlobalCloudManagerT<T>::MemoryOptimized);

    /**
     * @brief Recopie EN MEMOIRE les valeurs (T) du nuage défini par le nuage d'index passé en paramètre.
     *
     *        Crée un nouvel espace dans le nuage global de la taille du nuage d'index. Copie
     *        les valeurs (T) définies par le nuage d'index dans ce nouvel espace. Crée un nouveau nuage d'index non modifiable et l'enregistre auprès
     *        du gestionnaire d'index.
     *
     * @param pcr : le nuage enregistré à copier
     * @param optim : le type d'optimisation du nuage d'index
     * @return Retourne un nuage d'index correspondant à l'emplacement en mémoire des nouvelles valeurs (T). Ce nuage d'index n'est pas modifiable !
     */
    template< class IndexMemoryOptim, class IndexAccessOptim >
    CT_AbstractNotModifiableCIR copyCloud(CT_AbstractCIR cir, typename CT_AbstractGlobalCloudManagerT<T>::IndexOptimization optim = CT_AbstractGlobalCloudManagerT<T>::MemoryOptimized);

    /**
     * @brief Recopie EN MEMOIRE les valeurs (T) du nuage défini par le nuage d'index.
     *
     *        Crée un nouvel espace dans le nuage global de la taille du nuage d'index. Copie
     *        les valeurs (T) défini par le nuage d'index dans ce nouvel espace. Copie le nuage d'index et l'enregistre auprès
     *        du gestionnaire d'index.
     *
     * @param pcr : le nuage enregistré à copier
     * @param optim : le type d'optimisation du nuage d'index
     * @return Retourne un nuage d'index correspondant à l'emplacement en mémoire des nouvelles valeurs (T). Ce nuage d'index n'est pas modifiable !
     */
    template< class IndexMemoryOptim, class IndexAccessOptim >
    CT_AbstractNotModifiableCIR copyCloud(const CT_AbstractCloudIndexT<T> *index, typename CT_AbstractGlobalCloudManagerT<T>::IndexOptimization optim = CT_AbstractGlobalCloudManagerT<T>::MemoryOptimized);

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
    CT_AbstractNotModifiableCIR resizeCloudAndCloudIndex(CT_AbstractNotModifiableCIR cir, const size_t &newSize);

    /**
     * @brief globalAbstractCloud
     * @return Retourne le nuage global (il contient tout les nuages créés)
     */
    CT_AbstractCloud* globalAbstractCloud() const;

    /**
     * @brief globalAbstractCloud
     * @return Retourne le nuage global (il contient tout les nuages créés)
     */
    CT_AbstractCloudT<T>* globalAbstractCloudT() const;

    /**
     * @brief globalCloud
     * @return Retourne le nuage global (il contient tout les nuages créés)
     */
    CLOUD* globalCloud() const;

    /**
     * @brief Call this method when you begin to delete multiple cloud (to accelerate the process)
     *
     *        You can call this method multiple times, a counter is incremented : ++nCall;
     */
    void beginMultiDelete();

    /**
     * @brief Call this method when you have finished to delete multiple cloud
     *
     *        You can call this method multiple times, a counter is decremented : --nCall;
     *        When the counter is egal to zero all clouds that must be deleted will be delete with optimization.
     */
    void endMultiDelete();

protected:

    CLOUD                                   m_cloud;
    size_t                                  m_nCallToMultipleDelete;
    std::list< CT_GlobalCloudIndex >        m_multipleDeleteCollection; /*!<  a collection of index (begin, end) to delete */

    template< class IndexMemoryOptim, class IndexAccessOptim >
    CT_AbstractNotModifiableCIR createCIR(const size_t &beginIndex, const size_t &size, typename CT_AbstractGlobalCloudManagerT<T>::IndexOptimization optim);

    void resizeCloud(const size_t &newSize);

    static void staticDeleteCIR(CT_AbstractNotModifiableCloudIndexRegisteredT<T> *cir);

    bool internalUnregisterIndex(CT_AbstractCloudIndexRegisteredT<T> *cir, bool informCloudDeleted, bool deleteCIR);

    void deleteMultipleCloud();
};

#include "ct_cloud/tools/ct_globalcloudmanagert.hpp"

#endif // CT_GLOBALCLOUDMANAGERT_H

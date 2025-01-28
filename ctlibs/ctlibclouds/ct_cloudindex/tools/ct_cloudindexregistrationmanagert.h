#ifndef CT_CLOUDINDEXREGISTRATIONMANAGERT_H
#define CT_CLOUDINDEXREGISTRATIONMANAGERT_H

#include "ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanagert.h"

#include <QList>

template<typename U> class CT_AbstractGlobalCloudManagerT;

/**
 * A utility class that register cloud index of T (point, face, etc...) and sync indexes with
 * the global cloud of T when elements was deleted from them. This class can also merge two
 * contiguous cloud to one cloud index (create a CloudIndexLessMemory).
 */
template<typename T, class CloudIndexLessMemory>
class CT_CloudIndexRegistrationManagerT : public CT_AbstractCloudIndexRegistrationManagerT<T>
{
public:
    typedef typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractModifiableCIR      CT_AbstractModifiableCIR;
    typedef typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractNotModifiableCIR   CT_AbstractNotModifiableCIR;
    typedef typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractCIR                CT_AbstractCIR;

    CT_CloudIndexRegistrationManagerT(const CT_AbstractGlobalCloudManagerT<T> &gcManager);

    /**
     * @brief Enregistre un nuage d'index auprès de ce gestionnaire. Le nuage d'index sera toujours synchronisé
     *        par rapport au nuage global.
     *
     * @param index : le nuage d'index à enregistrer
     * @param id : l'itemdrawable qui va utiliser ce nuage d'index
     * @return Un objet enregistré contenant le nuage d'index.
     */
    CT_AbstractModifiableCIR registerIndex(CT_AbstractModifiableCloudIndexT<T> *index);

    /**
     * @brief Return a memory optimized cloud index that begin at the first index of the first cloud and finish at the end of the last cloud of the list
     * @param cir_collection : the list of cloud that is contiguous in memory
     * @return A memory optimized cloud index. This cloud index is not modifiable !
     */
    CT_AbstractNotModifiableCIR mergeCloudContiguous(const QList< CT_AbstractCIR > &cir_collection);

private:

    /**
     * @brief le gestionnaire du nuage global
     */
    CT_AbstractGlobalCloudManagerT<T>                           *m_gcManager;

    void syncModifiableIndexRegistered(const size_t &beginIndex, const size_t &size);
    void syncLessMemoryIndexRegistered(const size_t &beginIndex, const size_t &size);

    struct FindShiftRemoveContext
    {
    public:
        size_t beginIndex;
        size_t endIndex;
    };

    static bool staticFindIf(void *context, const size_t &value);
    static bool staticShiftIf(void *context, const size_t &value);
    static bool staticRemoveIf(void *context, const size_t &value);

protected:

    /**
     * @brief Called when the global cloud is imputed of elements between beginIndex and endIndex (endIndex = beginIndex+size).
     */
    void cloudDeleted(const size_t &beginIndex, const size_t &size);

    /**
     * @brief Called when the size of the global cloud has increased
     */
    void cloudAdded(const size_t &size) { Q_UNUSED(size) }
};

#include "ct_cloudindex/tools/ct_cloudindexregistrationmanagert.hpp"

#endif // CT_CLOUDINDEXREGISTRATIONMANAGERT_H

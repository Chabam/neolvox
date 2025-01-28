#ifndef CT_ABSTRACTGLOBALCLOUDMANAGERT_H
#define CT_ABSTRACTGLOBALCLOUDMANAGERT_H

#include "ct_cloud/tools/abstract/ct_abstractglobalcloudmanager.h"
#include "ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanagert.h"
#include "ct_global/ct_cloudscontext.h"

/**
 * Utility class who manage a global cloud of T (point, face, etc...). It will inform the CT_AbstractCloudIndexRegistrationManagerT and all
 * CT_AbstractCloudSyncToGlobalCloudManagerT when elements of the cloud is deleted or added. So we can sync index and
 * other cloud with this global cloud.
 */
template<typename T>
class CT_AbstractGlobalCloudManagerT : public CT_AbstractGlobalCloudManager
{
public:

    typedef typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractModifiableCIR         CT_AbstractModifiableCIR;
    typedef typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractNotModifiableCIR      CT_AbstractNotModifiableCIR;
    typedef typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractCIR                   CT_AbstractCIR;

    enum IndexOptimization {
        MemoryOptimized = 0,
        AccessSpeedOptimized
    };

    CT_AbstractGlobalCloudManagerT();

    /**
     * @brief Returns the global cloud of T
     */
    virtual CT_AbstractCloudT<T>* globalAbstractCloudT() const = 0;

    const QList< CT_AbstractCloudIndexRegisteredT<T>* >& getSyncedArray() const { return m_cirArray; }
    const QList< CT_AbstractCloudIndexRegisteredT<T>* >& getUnSyncedArray() const { return m_cirArrayUnsync; }

protected:

    QList< CT_AbstractCloudIndexRegisteredT<T>* >                   m_cirArray;
    QList< CT_AbstractCloudIndexRegisteredT<T>* >                   m_cirArrayUnsync;

    void shiftAllCloudIndexFrom(const int &from, const size_t &offset, const bool &negativeOffset);

    template<typename CI>
    std::vector< ct_index_type >* getInternalDataOfIndex(CI *index) const;

    template<typename CIR, typename CI>
    CIR* createNewIndexRegistered(CI *index) const;

    friend class CT_Mesh;

    // called by CT_Mesh (and must only be called by CT_Mesh)
    void setEnableSyncForCIR(CT_AbstractCloudIndexRegisteredT<T> *cir, bool enable);

    // called by inherited class to know index to ignore to sync
    bool nextIndexToIgnoreToSync(const size_t &currentIndex, size_t &beginIndex, size_t &endIndex);
};

#include "ct_cloud/tools/abstract/ct_abstractglobalcloudmanagert.hpp"

#endif // CT_ABSTRACTGLOBALCLOUDMANAGERT_H

#include "ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanagert.h"

template<typename T>
CT_AbstractCloudIndexRegistrationManagerT<T>::~CT_AbstractCloudIndexRegistrationManagerT()
{
    delete m_mutex;
}

template<typename T>
bool CT_AbstractCloudIndexRegistrationManagerT<T>::registerIndex(typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractModifiableCIR cir)
{
    m_cirArray.append(cir.data());

    return true;
}

template<typename T>
bool CT_AbstractCloudIndexRegistrationManagerT<T>::registerIndex(typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractNotModifiableCIR cir)
{
    m_cirMergedArray.append(cir.data());

    return true;
}

template<typename T>
bool CT_AbstractCloudIndexRegistrationManagerT<T>::unregisterIndex(const CT_AbstractModifiableCloudIndexRegisteredT<T> *index)
{
    QMutexLocker locker(m_mutex);

    if(m_cirArray.removeOne(const_cast<CT_AbstractModifiableCloudIndexRegisteredT<T>*>(index)))
        return true;

    return false;
}

template<typename T>
bool CT_AbstractCloudIndexRegistrationManagerT<T>::unregisterIndex(const CT_AbstractNotModifiableCloudIndexRegisteredT<T> *index)
{
    QMutexLocker locker(m_mutex);

    if(m_cirMergedArray.removeOne(const_cast<CT_AbstractNotModifiableCloudIndexRegisteredT<T>*>(index)))
        return true;

    return false;
}

template<typename T>
template<class CIR, class CI>
CIR* CT_AbstractCloudIndexRegistrationManagerT<T>::createNewIndexRegistered(CI *index) const
{
    CIR* cir = new CIR(index);
    cir->setCloudIndexRegistrationManager(this);

    return cir;
}

template<typename T>
QList< CT_AbstractModifiableCloudIndexRegisteredT<T>* > CT_AbstractCloudIndexRegistrationManagerT<T>::modifiableCloudIndexRegistered() const
{
    return m_cirArray;
}

template<typename T>
QList< CT_AbstractNotModifiableCloudIndexRegisteredT<T>* > CT_AbstractCloudIndexRegistrationManagerT<T>::notModifiableCloudIndexRegistered() const
{
    return m_cirMergedArray;
}

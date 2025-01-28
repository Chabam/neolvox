#include "ct_cloudindex/registered/abstract/ct_abstractmodifiablecloudindexregisteredt.h"

#include "ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanagert.h"

template<typename T>
CT_AbstractModifiableCloudIndexRegisteredT<T>::~CT_AbstractModifiableCloudIndexRegisteredT()
{
    this->cloudIndexRegistrationManager()->unregisterIndex(this);
}

template<typename T>
CT_AbstractModifiableCloudIndexT<T>* CT_AbstractModifiableCloudIndexRegisteredT<T>::operator->() const
{
    return m_modifiableIndex;
}

template<typename T>
CT_AbstractModifiableCloudIndexT<T>* CT_AbstractModifiableCloudIndexRegisteredT<T>::abstractModifiableCloudIndexT() const
{
    return m_modifiableIndex;
}

template<typename T>
CT_AbstractModifiableCloudIndex* CT_AbstractModifiableCloudIndexRegisteredT<T>::abstractModifiableCloudIndex() const
{
    return m_modifiableIndex;
}

template<typename T>
CT_AbstractModifiableCloudIndexRegisteredT<T>::CT_AbstractModifiableCloudIndexRegisteredT(CT_AbstractModifiableCloudIndexT<T> *ci) :  CT_AbstractCloudIndexRegisteredT<T>(ci)
{
    m_modifiableIndex = ci;
}

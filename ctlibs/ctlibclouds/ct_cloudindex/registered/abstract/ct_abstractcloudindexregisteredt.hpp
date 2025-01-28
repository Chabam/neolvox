#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"

template<typename T>
CT_AbstractCloudIndexRegisteredT<T>::CT_AbstractCloudIndexRegisteredT() : CT_AbstractCloudIndexRegistered()
{
    m_ci = nullptr;
    m_cirm = nullptr;
}

template<typename T>
CT_AbstractCloudIndexRegisteredT<T>::~CT_AbstractCloudIndexRegisteredT()
{
    if(mustAutoDeleteCloudIndex())
        delete m_ci;
}

template<typename T>
CT_AbstractCloudIndexT<T>* CT_AbstractCloudIndexRegisteredT<T>::operator->() const
{
    return m_ci;
}

template<typename T>
CT_AbstractCloudIndexT<T>* CT_AbstractCloudIndexRegisteredT<T>::abstractCloudIndexT() const
{
    return m_ci;
}

template<typename T>
CT_AbstractCloudIndex* CT_AbstractCloudIndexRegisteredT<T>::abstractCloudIndex() const
{
    return m_ci;
}

template<typename T>
CT_AbstractCloudIndexRegisteredT<T>::CT_AbstractCloudIndexRegisteredT(CT_AbstractCloudIndexT<T> *ci) : CT_AbstractCloudIndexRegistered()
{
    m_ci = ci;
    m_cirm = nullptr;
}

template<typename T>
void CT_AbstractCloudIndexRegisteredT<T>::setCloudIndexRegistrationManager(const CT_AbstractCloudIndexRegistrationManagerT<T> *cirm)
{
    m_cirm = const_cast<CT_AbstractCloudIndexRegistrationManagerT<T>*>(cirm);
}

template<typename T>
CT_AbstractCloudIndexRegistrationManagerT<T>* CT_AbstractCloudIndexRegisteredT<T>::cloudIndexRegistrationManager() const
{
    return m_cirm;
}

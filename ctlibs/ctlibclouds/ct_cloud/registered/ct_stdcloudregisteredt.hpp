#include "ct_cloud/registered/ct_stdcloudregisteredt.h"

template<typename T >
CT_StdCloudRegisteredT<T>::~CT_StdCloudRegisteredT()
{
    delete m_cloud;
}

template<typename T >
CT_AbstractCloud* CT_StdCloudRegisteredT<T>::abstractCloud() const
{
    return m_cloud;
}

template<typename T >
T* CT_StdCloudRegisteredT<T>::cloudT() const
{
    return m_cloud;
}

template<typename T >
CT_StdCloudRegisteredT<T>::CT_StdCloudRegisteredT(T *cloud) : CT_AbstractCloudRegistered()
{
    m_cloud = cloud;
}

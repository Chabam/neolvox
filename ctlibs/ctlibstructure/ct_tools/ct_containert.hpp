#include "ct_containert.h"

template<typename T, typename Locker>
CT_ContainerT<T, Locker>::CT_ContainerT(Locker& accessLocker) : m_accessLocker(accessLocker), m_autoDelete(true)
{
}

template<typename T, typename Locker>
CT_ContainerT<T, Locker>::~CT_ContainerT()
{
    clear();
}

template<typename T, typename Locker>
void CT_ContainerT<T, Locker>::append(T* element)
{
    m_elementsToKeep.append(element);
}

template<typename T, typename Locker>
void CT_ContainerT<T, Locker>::append(const QList<T*>& elements)
{
    m_elementsToKeep.append(elements);
}

template<typename T, typename Locker>
bool CT_ContainerT<T, Locker>::remove(T* element)
{
    if(m_elementsToKeep.removeOne(element) || m_elementsToBeRemovedLater.removeOne(element))
    {
        if(m_autoDelete)
            delete element;

        return true;
    }

    return false;
}

template<typename T, typename Locker>
int CT_ContainerT<T, Locker>::nToKeep() const
{
    return m_elementsToKeep.size();
}

template<typename T, typename Locker>
int CT_ContainerT<T, Locker>::nToBeRemovedLater() const
{
    return m_elementsToBeRemovedLater.size();
}

template<typename T, typename Locker>
int CT_ContainerT<T, Locker>::nInTotal() const
{
    return m_elementsToKeep.size() + m_elementsToBeRemovedLater.size();
}

template<typename T, typename Locker>
bool CT_ContainerT<T, Locker>::contains(const T* element) const
{
    return m_elementsToKeep.contains(const_cast<T*>(element));
}

template<typename T, typename Locker>
bool CT_ContainerT<T, Locker>::visitElementsToKeep(const typename CT_ContainerT<T, Locker>::ElementVisitor& visitor) const
{
    for(const T* element : m_elementsToKeep) {
        if(!visitor(element))
            return false;
    }

    return true;
}

template<typename T, typename Locker>
typename CT_ContainerT<T, Locker>::iterator CT_ContainerT<T, Locker>::begin()
{
    return m_elementsToKeep.begin();
}

template<typename T, typename Locker>
typename CT_ContainerT<T, Locker>::iterator CT_ContainerT<T, Locker>::end()
{
    return m_elementsToKeep.end();
}

template<typename T, typename Locker>
typename CT_ContainerT<T, Locker>::const_iterator CT_ContainerT<T, Locker>::constBegin() const
{
    return m_elementsToKeep.constBegin();
}

template<typename T, typename Locker>
typename CT_ContainerT<T, Locker>::const_iterator CT_ContainerT<T, Locker>::constEnd() const
{
    return m_elementsToKeep.constEnd();
}

template<typename T, typename Locker>
bool CT_ContainerT<T, Locker>::visitElementsToBeRemovedLater(const typename CT_ContainerT<T, Locker>::ElementVisitor& visitor) const
{
    for(const T* element : m_elementsToBeRemovedLater) {
        if(!visitor(element))
            return false;
    }

    return true;
}


template<typename T, typename Locker>
bool CT_ContainerT<T, Locker>::visitAllElements(const typename CT_ContainerT<T, Locker>::ElementVisitor& visitor) const
{
    if(!visitElementsToKeep(visitor))
        return false;

    return visitElementsToBeRemovedLater(visitor);
}

template<typename T, typename Locker>
void CT_ContainerT<T, Locker>::setToBeRemovedLater(const T* element)
{
    if(m_elementsToKeep.removeOne(const_cast<T*>(element)))
        m_elementsToBeRemovedLater.append(const_cast<T*>(element));
}

template<typename T, typename Locker>
void CT_ContainerT<T, Locker>::setToKeep(const T* element)
{
    if(m_elementsToBeRemovedLater.removeOne(const_cast<T*>(element)))
        m_elementsToKeep.append(const_cast<T*>(element));
}

template<typename T, typename Locker>
void CT_ContainerT<T, Locker>::setAutoDelete(const bool& autoDelete)
{
    m_autoDelete = autoDelete;
}

template<typename T, typename Locker>
bool CT_ContainerT<T, Locker>::mustAutoDelete() const
{
    return m_autoDelete;
}

template<typename T, typename Locker>
void CT_ContainerT<T, Locker>::clear()
{
    clearElementsToKeep();
    clearElementsToBeRemovedLater();
}

template<typename T, typename Locker>
void CT_ContainerT<T, Locker>::clearElementsToKeep()
{
    if(m_autoDelete)
        qDeleteAll(m_elementsToKeep);

    m_elementsToKeep.clear();
}

template<typename T, typename Locker>
void CT_ContainerT<T, Locker>::clearElementsToBeRemovedLater()
{
    if(m_autoDelete)
        qDeleteAll(m_elementsToBeRemovedLater);

    m_elementsToBeRemovedLater.clear();
}

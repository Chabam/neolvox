#ifndef CT_SHAREDPOINTER_H
#define CT_SHAREDPOINTER_H

#include "stddef.h"

/**
 * @brief Special type of shared pointer because you can disable the deletion of the object (call method "take"). It
 *        will be very interesting if you want to pass the object to another type of shared_ptr like boost::shared_ptr or std::shared_ptr !
 */
template<typename T>
class CT_SharedPointer
{
public:
    CT_SharedPointer(T* ptr, bool autoDelete = true)
    {
        m_ptr = ptr;
        m_nRef = new size_t;
        m_deletePtr = new bool;
        *m_deletePtr = autoDelete;

        increment();
    }

    CT_SharedPointer(const CT_SharedPointer<T> &other)
    {
        m_ptr = other.m_ptr;
        m_nRef = other.m_nRef;
        m_deletePtr = other.m_deletePtr;
        increment();
    }

    ~CT_SharedPointer()
    {
        if(decrement() == 0)
            clear();
    }

    const T& operator*() const { return *m_ptr; }
    const T* operator->() const { return m_ptr; }
    T& operator*() { return *m_ptr; }
    T* operator->() { return m_ptr; }
    T* get() const { return m_ptr; }

    T* take()
    {
        *m_deletePtr = false;
        return m_ptr;
    }

    bool autoDelete() const
    {
        return (*m_deletePtr);
    }

    CT_SharedPointer<T>& operator = (const CT_SharedPointer<T>& other)
    {
        // Assignment operator
        if (this != &other) // Avoid self assignment
        {
            // Decrement the old reference count
            // if reference become zero delete the old data
            if(decrement() == 0)
                clear();

            // Copy the data and reference pointer
            // and increment the reference count
            m_ptr = other.m_ptr;
            m_nRef = other.m_nRef;
            m_deletePtr = other.m_deletePtr;
            increment();
        }

        return *this;
    }

private:
    T*      m_ptr;
    size_t  *m_nRef;
    bool    *m_deletePtr;

    void increment()
    {
        ++(*m_nRef);
    }

    size_t decrement()
    {
        --(*m_nRef);
        return (*m_nRef);
    }

    void clear()
    {
        delete m_nRef;

        if(m_deletePtr)
            delete m_ptr;

        delete m_deletePtr;
    }
};

#endif // CT_SHAREDPOINTER_H

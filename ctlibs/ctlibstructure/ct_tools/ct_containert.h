#ifndef CT_CONTAINERT_H
#define CT_CONTAINERT_H

#include <QList>
#include <functional>

/**
 * @brief A container that conatins pointer to specific elements.
 *
 *        The container use two collection. One for elements that must be removed later
 *        and one for elements that must be keeped.
 */
template<typename T, typename Locker>
class CT_ContainerT
{
public:
    using ElementType = T;
    using ElementVisitor = std::function<bool(const ElementType*)>;
    using iterator = typename QList<ElementType*>::iterator;
    using const_iterator = typename QList<ElementType*>::const_iterator;

    CT_ContainerT(Locker& accessLocker);
    ~CT_ContainerT();

    void lock() { m_accessLocker.lockAccess(); }
    void unlock() { m_accessLocker.unlockAccess(); }

    /**
     * @brief Add a T in the collection of elements that must be keeped
     * @param element : the element to add
     * @warning No verification is made to check if it was already contained or not in this container !
     */
    void append(T* element);

    /**
     * @brief Add a list of T in the collection of elements that must be keeped
     * @param elements : elements to add
     * @warning No verification is made to check if it was already contained or not in this container !
     */
    void append(const QList<T*>& elements);

    /**
     * @brief Remove the specified element from this container (it can exist in one of the two collection). It will be deleted from memory if mustAutoDelete() returns true.
     * @param element : the element to remove
     * @return false if the element is not in this container
     */
    bool remove(T* element);

    /**
     * @brief Returns the number of elements that must not be removed later
     */
    int nToKeep() const;

    /**
     * @brief Returns the number of elements that must be removed later
     */
    int nToBeRemovedLater() const;

    /**
     * @brief Returns the number of elements that must be keeped + the number of elements that must be removed later
     */
    int nInTotal() const;

    /**
     * @brief Returns true if the element already exist in the collection of elements that must be keeped
     */
    bool contains(const T* element) const;

    /**
     * @brief Visit elements that must not be removed later
     * @param visitor : the visitor to use
     * @return Returns true if no elements has been visited otherwise returns the result of the visitor.
     */
    bool visitElementsToKeep(const ElementVisitor& visitor) const;

    /**
     * @brief Returns a STL-style iterator pointing to the first element in the collection of elements that must be keeped.
     */
    iterator begin();

    /**
     * @brief Returns a STL-style iterator pointing to the imaginary element after the element item in the collection of elements that must be keeped.
     */
    iterator end();

    /**
     * @brief Returns a const STL-style iterator pointing to the first element in the collection of elements that must be keeped.
     */
    const_iterator constBegin() const;

    /**
     * @brief Returns a const STL-style iterator pointing to the imaginary element after the element item in the collection of elements that must be keeped.
     */
    const_iterator constEnd() const;

    /**
     * @brief Visit elements that must be removed later
     * @param visitor : the visitor to use
     * @return Returns true if no elements has been visited otherwise returns the result of the visitor.
     */
    bool visitElementsToBeRemovedLater(const ElementVisitor& visitor) const;

    /**
     * @brief Visit all elements
     * @param visitor : the visitor to use
     * @return Returns true if no elements has been visited otherwise returns the result of the visitor.
     */
    bool visitAllElements(const ElementVisitor& visitor) const;

    /**
     * @brief Swap the element from the "keep" collection to the "remove later" collection
     */
    void setToBeRemovedLater(const T* element);

    /**
     * @brief Swap the element from the "remove later" collection to the "keep" collection
     */
    void setToKeep(const T* element);

    /**
     * @brief Set if must auto delete or not elements in this container
     */
    void setAutoDelete(const bool& autoDelete);

    /**
     * @brief Returns true if must auto delete elements in this container
     */
    bool mustAutoDelete() const;

    /**
     * @brief Remove all elements from this container
     */
    void clear();

    /**
     * @brief Remove all elements to keep from this container
     */
    void clearElementsToKeep();

    /**
     * @brief Remove all elements to be removed later from this container
     */
    void clearElementsToBeRemovedLater();

private:
    Locker&     m_accessLocker;
    QList<T*>   m_elementsToKeep;
    QList<T*>   m_elementsToBeRemovedLater;
    bool        m_autoDelete;
};

#include "ct_containert.hpp"

#endif // CT_CONTAINERT_H

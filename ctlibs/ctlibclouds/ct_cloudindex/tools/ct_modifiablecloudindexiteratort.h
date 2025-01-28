#ifndef CT_MODIFIABLECLOUDINDEXITERATORT_H
#define CT_MODIFIABLECLOUDINDEXITERATORT_H

#include <cstddef>

template<typename T> class CT_AbstractModifiableCloudIndexT;

template< typename T>
class CT_ModifiableCloudIndexIteratorT
{
public:
    typedef CT_ModifiableCloudIndexIteratorT<T> self_type;

    CT_ModifiableCloudIndexIteratorT(const CT_AbstractModifiableCloudIndexT<T> &index, const size_t &startPos = 0)
    {
        m_index = (CT_AbstractModifiableCloudIndexT<T>*)&index;
        m_currentPos = startPos;
    }

    inline size_t cIndex() const { return (*m_index)[m_currentPos]; }
    inline void cIndexReplace(const ct_index_type &newIndex, const bool &verifyRespectSort = true) { m_index->replaceIndex(m_currentPos, newIndex, verifyRespectSort); }

    inline T& cT() { return operator*(); }

    inline T& operator*() { return m_index->tAt(m_currentPos); }
    inline T* operator->() { return &(operator*()); }

    self_type& operator++() { ++m_currentPos; return *this; }
    self_type operator+(const size_t &i) { self_type tmp(*this); tmp.m_currentPos += i; return tmp; }
    self_type& operator+=(const size_t &i) { m_currentPos += i; return *this; }

    self_type& operator--() { --m_currentPos; return *this; }
    self_type operator-(const size_t &i) { self_type tmp(*this); tmp.m_currentPos -= i; return tmp; }
    self_type& operator-=(const size_t &i) { m_currentPos -= i; return *this; }

    bool operator==(const self_type &other) const { return ((m_index == other.m_index) && (m_currentPos == other.m_currentPos)); }
    bool operator!=(const self_type &other) const { return ((m_index != other.m_index) ||    (m_currentPos != other.m_currentPos)); }

private:
    CT_AbstractModifiableCloudIndexT<T>     *m_index;
    size_t                                  m_currentPos;
};

#endif // CT_MODIFIABLECLOUDINDEXITERATORT_H

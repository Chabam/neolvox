#ifndef CT_HANDLEITERATORT_H
#define CT_HANDLEITERATORT_H

#include <iterator>

template<typename IteratorT>
class CT_HandleIteratorT {
public:
    using IteratorType = IteratorT;

    CT_HandleIteratorT(IteratorT begin, IteratorT end) : m_begin(begin), m_end(end) {}

    IteratorT begin() { return m_begin; }
    IteratorT end() { return m_end; }

    /**
     * @brief Count the number of element to iterate (may be it must iterate over all element
     *        first to determine the result) and return it. If you want just know if there is
     *        or not element to iterate over, prefer call the method "isEmpty()".
     */
    int count() const {
        IteratorT it = m_begin;
        IteratorT end = m_end;

        return std::distance(it, end);
    }

    /**
     * @brief Returns true if there is no element to iterate over
     */
    bool isEmpty() const { return (m_begin == m_end); }

private:
    IteratorT m_begin;
    IteratorT m_end;
};

#endif // CT_HANDLEITERATORT_H

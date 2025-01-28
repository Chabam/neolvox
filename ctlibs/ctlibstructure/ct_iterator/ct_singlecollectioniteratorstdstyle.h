#ifndef CT_SINGLECOLLECTIONITERATORSTDSTYLE_H
#define CT_SINGLECOLLECTIONITERATORSTDSTYLE_H

#include <iterator>
#include <vector>

template<class ItemT>
class CT_SingleCollectionIteratorStdStyle : public std::iterator<std::input_iterator_tag, ItemT*> {
public:
    using self_type = CT_SingleCollectionIteratorStdStyle<ItemT>;

    CT_SingleCollectionIteratorStdStyle() : m_currentIndex(-1), m_currentValue(nullptr) {}

    template<typename InfoCollection>
    static self_type create(const InfoCollection& c) {
        self_type it;

        std::copy(c.begin(), c.end(), std::back_inserter(it.m_collections));

        if(!it.m_collections.empty()) {
            it.m_currentIndex = 0;
            it.m_currentValue = it.m_collections[0];
        }

        return it;
    }

    template<typename InfoCollectionIterator>
    static self_type create(InfoCollectionIterator begin, InfoCollectionIterator end) {
        self_type it;

        std::copy(begin, end, std::back_inserter(it.m_collections));

        if(!it.m_collections.empty()) {
            it.m_currentIndex = 0;
            it.m_currentValue = it.m_collections[0];
        }

        return it;
    }

    self_type& operator++() {
        if(m_currentValue == nullptr)
            return *this;

        if(m_currentIndex == -1) {
            m_currentValue = nullptr;
            return *this;
        }

        ++m_currentIndex;

        if(m_currentIndex < static_cast<int>(m_collections.size()))
        {
            m_currentValue = m_collections[m_currentIndex]; // got it !
        }
        else
        {
            m_currentIndex = -1;
            m_currentValue = nullptr;
        }

        return *this;
    }

    typename std::iterator<std::input_iterator_tag, ItemT*>::reference operator*() { return m_currentValue; }
    typename std::iterator<std::input_iterator_tag, ItemT*>::pointer operator->() { return &m_currentValue; }
    bool operator==(const self_type& rhs) { return m_currentValue == rhs.m_currentValue; }
    bool operator!=(const self_type& rhs) { return m_currentValue != rhs.m_currentValue; }

private:
    std::vector<ItemT*>         m_collections;
    int                         m_currentIndex;
    ItemT*                      m_currentValue;
};

#endif // CT_SINGLECOLLECTIONITERATORSTDSTYLE_H

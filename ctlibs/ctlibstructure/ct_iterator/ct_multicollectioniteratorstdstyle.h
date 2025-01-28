#ifndef CT_MULTICOLLECTIONITERATORSTDSTYLE_H
#define CT_MULTICOLLECTIONITERATORSTDSTYLE_H

#include <iterator>
#include <vector>
#include <QPair>

template<typename IteratorT, class ItemT>
class CT_MultiCollectionIteratorStdStyle : public std::iterator<std::input_iterator_tag, ItemT*> {
public:
    using self_type = CT_MultiCollectionIteratorStdStyle<IteratorT, ItemT>;

    CT_MultiCollectionIteratorStdStyle() : m_currentIndex(-1), m_currentValue(nullptr) {}

    template<typename IteratorInfoCollection>
    static self_type create(const IteratorInfoCollection& c) {
        self_type it;

        std::copy(c.begin(), c.end(), std::back_inserter(it.m_collections));

        if(!it.m_collections.empty()) {
            IteratorInfo& its = it.m_collections[0];

            if(its.first != its.second) {
                it.m_currentIndex = 0;
                it.m_currentValue = static_cast<ItemT*>(*it.m_collections[it.m_currentIndex].first); // got it !
            }
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

        IteratorInfo& its = m_collections[m_currentIndex];
        ++its.first;

        // if we have reached the end of the current collection
        if(its.first == its.second) {
            // we go to the next collection
            ++m_currentIndex;

            if(m_currentIndex < static_cast<int>(m_collections.size()))
                m_currentValue = static_cast<ItemT*>(*m_collections[m_currentIndex].first); // got it !
            else
                m_currentValue = nullptr;
        } else {
            m_currentValue = static_cast<ItemT*>(*its.first); // got it !
        }

        return *this;
    }

    typename std::iterator<std::input_iterator_tag, ItemT*>::reference operator*() { return m_currentValue; }
    typename std::iterator<std::input_iterator_tag, ItemT*>::pointer operator->() { return &m_currentValue; }
    bool operator==(const self_type& rhs) { return m_currentValue == rhs.m_currentValue; }
    bool operator!=(const self_type& rhs) { return m_currentValue != rhs.m_currentValue; }

private:
    using IteratorInfo = QPair<IteratorT, IteratorT>;

    std::vector<IteratorInfo>   m_collections;
    int                         m_currentIndex;
    ItemT*                      m_currentValue;
};

#endif // CT_MULTICOLLECTIONITERATORSTDSTYLE_H

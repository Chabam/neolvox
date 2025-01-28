#ifndef CT_SINGLEMODELITERATORSTDSTYLEFORRESULTGROUP_H
#define CT_SINGLEMODELITERATORSTDSTYLEFORRESULTGROUP_H

#include "ct_model/outModel/abstract/ct_outabstractmodel.h"
#include "ct_result/ct_resultgroup.h"
#include "ct_tools/ct_handleiteratort.h"

#include <iterator>

template<class ValueT, typename ParentT = CT_AbstractItem>
class CT_SingleModelIteratorStdStyleForResultGroup : public std::iterator<std::input_iterator_tag, ValueT*> {
public:
    using OutModelType = CT_OutAbstractModel;
    using self_type = CT_SingleModelIteratorStdStyleForResultGroup<ValueT, ParentT>;
    using ParentType = ParentT;

    static CT_HandleIteratorT<self_type> createCompleteIterator(const OutModelType* outModel) {
        return CT_HandleIteratorT<self_type>(self_type(outModel), self_type());
    }

    CT_SingleModelIteratorStdStyleForResultGroup() : m_result(nullptr), m_currentValue(nullptr), m_currentParent(nullptr), m_currentIndexInHierarchy(-1) {}

    CT_SingleModelIteratorStdStyleForResultGroup(const OutModelType* outModel) : m_result(nullptr), m_currentValue(nullptr), m_currentParent(nullptr), m_currentIndexInHierarchy(-1) {
        Q_ASSERT(outModel != nullptr);
        if (outModel == nullptr) {qDebug() << "CT_SingleModelIteratorStdStyleForResultGroup::CT_SingleModelIteratorStdStyleForResultGroup" << ", " << "outModel == nullptr";}

        m_result = static_cast<CT_ResultGroup*>(outModel->result());

        Q_ASSERT(m_result != nullptr);
        if (m_result == nullptr) {qDebug() << "CT_SingleModelIteratorStdStyleForResultGroup::CT_SingleModelIteratorStdStyleForResultGroup" << ", " << "m_result == nullptr";}

        OutModelType* m = const_cast<OutModelType*>(outModel);

        while(m != nullptr) {
            m_modelsHierarchy.prepend(m);
            m = static_cast<OutModelType*>(m->parentModel());
        }

        Q_ASSERT(m_result->model() == m_modelsHierarchy[0]);
        if (m_result->model() != m_modelsHierarchy[0]) {qDebug() << "CT_SingleModelIteratorStdStyleForResultGroup::CT_SingleModelIteratorStdStyleForResultGroup" << ", " << "m_result->model() != m_modelsHierarchy[0]";}

        if(m_modelsHierarchy.size() > 1) {

            m_iteratorHierarchy.resize(m_modelsHierarchy.size());

            CT_AbstractItem::ChildrensCollection collection = m_result->childrensThatUseOutModel(m_modelsHierarchy[1]);
            childrens_iterator begin = collection.begin();
            childrens_iterator end = collection.end();

            m_currentIndexInHierarchy = 0;

            // go down in the hierarchy
            bool haveFoundWhatWeSearch = goDownInHierarchy(begin, end);

            // if we have not found what we search we must go up in the hierarchy and go down
            // until we find what we search or until we went through the entire collection
            while(!haveFoundWhatWeSearch
                  && (m_currentIndexInHierarchy != 0)) {
                haveFoundWhatWeSearch = goToNextInHierarchy();
            }

            if(haveFoundWhatWeSearch) {
                if(m_currentIndexInHierarchy > 0)
                    m_currentParent = static_cast<ParentType*>(*m_iteratorHierarchy[m_currentIndexInHierarchy-1].first);

                m_currentValue = static_cast<ValueT*>(*m_iteratorHierarchy[m_currentIndexInHierarchy].first);
            }
        } else {
            m_currentValue = dynamic_cast<ValueT*>(m_result);
        }
    }

    self_type& operator++() {
        if(m_currentValue == nullptr)
            return *this;

        if(m_currentIndexInHierarchy == -1) {
            m_currentValue = nullptr;
            return *this;
        }

        // if we are here m_currentIndexInHierarchy is always equal to the last column of the hierarchy
        IteratorInfo& its = m_iteratorHierarchy[m_currentIndexInHierarchy];
        ++its.first;

        // if we have reached the end of the collection
        if(its.first == its.second) {
            // we go up in hierarchy
            --m_currentIndexInHierarchy;

            // and search the next (return true if found, false if there is no more element that we search in all hierarchy)
            if(goToNextInHierarchy()) {
                if(m_currentIndexInHierarchy > 0)
                    m_currentParent = static_cast<ParentType*>(*m_iteratorHierarchy[m_currentIndexInHierarchy-1].first);

                m_currentValue = static_cast<ValueT*>(*m_iteratorHierarchy[m_currentIndexInHierarchy].first); // got it !
            }
            else
                m_currentValue = nullptr;
        } else {
            m_currentValue = static_cast<ValueT*>(*its.first); // got it !
        }

        return *this;
    }

    typename std::iterator<std::input_iterator_tag, ValueT*>::reference operator*() { return m_currentValue; }
    typename std::iterator<std::input_iterator_tag, ValueT*>::pointer operator->() { return &m_currentValue; }
    bool operator==(const self_type& rhs) { return m_currentValue == rhs.m_currentValue; }
    bool operator!=(const self_type& rhs) { return m_currentValue != rhs.m_currentValue; }

    ParentType* currentParent() { return m_currentParent; }

private:
    // begin, end
    using childrens_iterator = CT_AbstractItem::ChildrensCollection::const_iterator;
    using IteratorInfo = QPair<childrens_iterator, childrens_iterator>;

    CT_ResultGroup*             m_result;
    ValueT*                     m_currentValue;
    ParentType*                 m_currentParent;
    QVector<OutModelType*>      m_modelsHierarchy;
    QVector<IteratorInfo>       m_iteratorHierarchy;
    int                         m_currentIndexInHierarchy;

    bool goDownInHierarchy(childrens_iterator begin, childrens_iterator end) {
        while(((m_currentIndexInHierarchy+1) != m_modelsHierarchy.size())
                && (begin != end)) {

            ++m_currentIndexInHierarchy;
            m_iteratorHierarchy[m_currentIndexInHierarchy] = qMakePair(begin, end);

            if((m_currentIndexInHierarchy+1) != m_modelsHierarchy.size()) {
                CT_AbstractItem* currentElement = (*begin);
                auto collection = currentElement->childrensThatUseOutModel(m_modelsHierarchy[m_currentIndexInHierarchy+1]);
                begin = collection.begin();
                end = collection.end();
            }
        }

        // if we are in the last column of the hierarchy we have found what we search
        return ((m_currentIndexInHierarchy+1) == m_modelsHierarchy.size());
    }

    bool goToNextInHierarchy() {

        bool haveFoundWhatWeSearch = false;

        while(!haveFoundWhatWeSearch
                && (m_currentIndexInHierarchy != 0)) {

            IteratorInfo& its = m_iteratorHierarchy[m_currentIndexInHierarchy];
            ++its.first;

            if(its.first != its.second) {
                CT_AbstractItem* currentElement = (*its.first);
                CT_AbstractItem::ChildrensCollection collection = currentElement->childrensThatUseOutModel(m_modelsHierarchy[m_currentIndexInHierarchy+1]);
                childrens_iterator begin = collection.begin();
                childrens_iterator end = collection.end();

                haveFoundWhatWeSearch = goDownInHierarchy(begin, end);
            } else {
                --m_currentIndexInHierarchy;
            }
        }

        return haveFoundWhatWeSearch;
    }
};

#endif // CT_SINGLEMODELITERATORSTDSTYLEFORRESULTGROUP_H

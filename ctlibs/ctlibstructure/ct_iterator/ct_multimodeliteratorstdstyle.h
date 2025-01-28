#ifndef CT_MULTIMODELITERATORSTDSTYLE_H
#define CT_MULTIMODELITERATORSTDSTYLE_H

#include "ct_model/outModel/abstract/ct_outabstractmodel.h"

#include <iterator>
#include <QVector>

template<class SingleModelIteratorStdStyleT, class ValueT>
class CT_MultiModelIteratorStdStyle : public std::iterator<std::input_iterator_tag, ValueT*>
{
public:
    using OutModelType = CT_OutAbstractModel;
    using SingleModelIt = SingleModelIteratorStdStyleT;
    using self_type = CT_MultiModelIteratorStdStyle<SingleModelIteratorStdStyleT, ValueT>;

    CT_MultiModelIteratorStdStyle() : m_currentValue(nullptr), mCurrentParent(nullptr), m_currentOutModelIndex(-1) {}

    template<class OutModelCollectionIt>
    static CT_MultiModelIteratorStdStyle create(OutModelCollectionIt first, OutModelCollectionIt last) {
        CT_MultiModelIteratorStdStyle it;

        it.m_outModels.resize(std::distance(first, last));

        if(!it.m_outModels.isEmpty()) {
            int i = 0;
            while(first != last) {
                it.m_outModels[i++] = (*first);
                ++first;
            }

            it.m_currentOutModelIndex = 0;
            it.m_currentSingleModelIt = SingleModelIt(it.m_outModels[it.m_currentOutModelIndex]);
            it.m_currentValue = (*it.m_currentSingleModelIt);

            while(it.m_currentValue == nullptr) {

                ++it.m_currentOutModelIndex;

                if(it.m_currentOutModelIndex >= it.m_outModels.size())
                    return it;

                it.m_currentSingleModelIt = SingleModelIt(it.m_outModels[it.m_currentOutModelIndex]);
                it.m_currentValue = (*it.m_currentSingleModelIt);
            }

            it.mCurrentParent = it.m_currentSingleModelIt.currentParent();
            ++it.m_currentSingleModelIt;
        }

        return it;
    }

    self_type& operator++() {
        if(m_currentValue == nullptr)
            return *this;

        m_currentValue = (*m_currentSingleModelIt);

        while(m_currentValue == nullptr) {

            ++m_currentOutModelIndex;

            if(m_currentOutModelIndex >= m_outModels.size())
                return *this;

            m_currentSingleModelIt = SingleModelIt(m_outModels[m_currentOutModelIndex]);
            m_currentValue = (*m_currentSingleModelIt);
        };

        mCurrentParent = m_currentSingleModelIt.currentParent();
        ++m_currentSingleModelIt;

        return *this;
    }

    typename std::iterator<std::input_iterator_tag, ValueT*>::reference operator*() { return m_currentValue; }
    typename std::iterator<std::input_iterator_tag, ValueT*>::pointer operator->() { return &m_currentValue; }
    bool operator==(const self_type& rhs) { return m_currentValue == rhs.m_currentValue; }
    bool operator!=(const self_type& rhs) { return m_currentValue != rhs.m_currentValue; }

    typename SingleModelIteratorStdStyleT::ParentType* currentParent() { return mCurrentParent; }

private:
    ValueT*                                             m_currentValue;
    typename SingleModelIteratorStdStyleT::ParentType*  mCurrentParent;
    SingleModelIt                                       m_currentSingleModelIt;
    int                                                 m_currentOutModelIndex;
    QVector<OutModelType*>                              m_outModels;
};

#endif // CT_MULTIMODELITERATORSTDSTYLE_H

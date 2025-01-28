#ifndef CT_CLOUDINDEXITERATORT_H
#define CT_CLOUDINDEXITERATORT_H

#include <QSharedPointer>
#include "ct_global/ct_cloudscontext.h"
#include "ct_global/ct_repositorymanager.h"
#include "ct_global/ct_repository.h"
#include "ct_cloud/abstract/ct_abstractcloudt.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregistered.h"

template <typename T> class CT_AbstractCloudIndexT;

template<typename T>
class CT_CloudIndexIteratorT
{
public:
    typedef CT_CloudIndexIteratorT<T> self_type;

    CT_CloudIndexIteratorT(const CT_AbstractCloudIndexT<T> *index)
    {
        m_index = (CT_AbstractCloudIndexT<T>*)index;
        m_startPos = 0;
        m_endPos = (m_index == nullptr) ? 0 : m_index->size();

        toFront();
    }

    CT_CloudIndexIteratorT(QSharedPointer<CT_AbstractCloudIndexRegistered> cir)
    {
        if(cir.data() == nullptr)
            m_index = nullptr;
        else
            m_index = dynamic_cast<CT_AbstractCloudIndexT<T>*>(cir->abstractCloudIndex());

        m_startPos = 0;
        m_endPos = (m_index == nullptr) ? 0 : m_index->size();

        toFront();
    }

    inline size_t size() const { return m_index->size(); }


    inline void toFront() { m_currentPos = m_startPos; m_atFront = true; }

    inline bool hasNext() const { if(m_startPos == m_endPos) { return false; } if(m_atFront) { return m_currentPos < m_endPos; } else { return m_currentPos < (m_endPos-1); } }

    inline CT_CloudIndexIteratorT<T>& next() { if(m_atFront || (m_startPos == m_endPos)) { m_currentPos = m_startPos; m_atFront = false; } else { ++m_currentPos; } return *this; }


    inline void toBack() { m_currentPos = m_endPos; m_atFront = false; }

    inline bool hasPrevious() const { return m_currentPos > m_startPos; }

    inline CT_CloudIndexIteratorT<T>& previous() { if(m_currentPos > m_startPos) { --m_currentPos; } return *this; }


    inline CT_CloudIndexIteratorT<T>& jump(size_t n)
    {
        if(m_startPos == m_endPos)
            return *this;

        // if at front
        if(m_atFront) {

            if(n != 0) {
                m_currentPos = m_startPos+(n-1); // go to start+n-1
                m_atFront = false;
            }

            // if over the end
            if(m_currentPos > (m_endPos-1))
                m_currentPos = (m_endPos-1); // go to end

        // else if we jump over the end
        } else if((m_currentPos+n) >= (m_endPos-1)) {
            m_currentPos = (m_endPos-1); // go to end
            m_atFront = false;
        } else {
            m_currentPos += n;
            m_atFront = false;
        }

        return *this;
    }

    inline size_t cIndex() { return (*m_index)[m_currentPos]; }
    const T& cT() const { return operator*(); }
    T& cT() { return operator*(); }

    const T& operator*() const { return PS_REPOSITORY->globalCloud<T>()->constTAt(cIndex()); }
    const T* operator->() const { return &(operator*()); }

    T& operator*() { return PS_REPOSITORY->globalCloud<T>()->tAt(cIndex()); }
    T* operator->() { return &(operator*()); }

private:
    CT_AbstractCloudIndexT<T>   *m_index;
    size_t                      m_startPos;
    size_t                      m_currentPos;
    size_t                      m_endPos;
    bool                        m_atFront;
};

#endif // CT_CLOUDINDEXITERATORT_H

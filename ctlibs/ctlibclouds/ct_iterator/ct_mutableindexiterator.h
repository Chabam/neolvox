#ifndef CT_MUTABLEINDEXITERATOR_H
#define CT_MUTABLEINDEXITERATOR_H

#include "ct_cloudindex/abstract/ct_abstractmodifiablecloudindex.h"
#include "ct_defines.h"

/**
 * @brief Class to iterate over indexes and modify it.
 * @warning Use this class only when you create a new vector of indexes and you want to modify it (per example for mesh)
 */
class CTLIBCLOUDS_EXPORT CT_MutableIndexIterator
{
public:
    /**
     * @brief Create an iterator for a modifiable cloud index
     */
    CT_MutableIndexIterator(const CT_AbstractModifiableCloudIndex *mci);

    /**
     * @brief Create an iterator for a registered modifiable cloud index
     */
    CT_MutableIndexIterator(const CT_MCIR mcir);

    /**
     * @brief Returns true if has a next index
     */
    inline bool hasNext() const { if(m_startPos == m_endPos) { return false; } if(m_atFront) { return m_currentPos < m_endPos; } else { return m_currentPos < (m_endPos-1); } }

    /**
     * @brief Go to next index
     */
    inline CT_MutableIndexIterator& next() { if(m_atFront || (m_startPos == m_endPos)) { m_currentPos = m_startPos; m_atFront = false; } else { ++m_currentPos; } return *this; }

    /**
     * @brief Returns true if has a previous index
     */
    inline bool hasPrevious() const { return m_currentPos > m_startPos; }

    /**
     * @brief Go to previous index
     */
    inline CT_MutableIndexIterator& previous() { if(m_currentPos > m_startPos) { --m_currentPos; } return *this; }

    /**
     * @brief Jump from (current position) to (current position + n)
     * @warning call method "hasNext()" after this method
     */
    inline CT_MutableIndexIterator& jump(size_t n)
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

    /**
     * @brief Go to front of the collection
     */
    inline void toFront() { m_currentPos = m_startPos; m_atFront = true; }

    /**
     * @brief Go to end of the collection
     */
    inline void toBack() { m_currentPos = m_endPos; m_atFront = false; }

    /**
     * @brief Returns the size of the cloud index
     */
    inline size_t size() const { return m_index->size(); }

    /**
     * @brief Return current global index
     */
    inline size_t currentGlobalIndex() const { return (*m_index)[m_currentPos]; }
    inline size_t cIndex() const { return currentGlobalIndex(); }

    /**
     * @brief Replace the current global index by a new index
     *
     * @param newIndex : the new index
     * @param verifyRespectSort : set to false if you want to accelerate the process and you are VERY sure to respect the sort of the collection
     */
    inline void replaceCurrentGlobalIndex(const ct_index_type &newIndex, const bool &verifyRespectSort = true) { m_index->replaceIndex(m_currentPos, newIndex, verifyRespectSort); }

private:
    CT_AbstractModifiableCloudIndex     *m_index;
    size_t                              m_startPos;
    size_t                              m_currentPos;
    size_t                              m_endPos;
    bool                                m_atFront;
};

#endif // CT_MUTABLEINDEXITERATOR_H

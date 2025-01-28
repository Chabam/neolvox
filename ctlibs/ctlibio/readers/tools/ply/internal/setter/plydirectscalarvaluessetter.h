#ifndef PLYDIRECTSCALARVALUESSETTER_H
#define PLYDIRECTSCALARVALUESSETTER_H

#include "plyabstractvaluessetter.h"

/**
 * @brief This class will set values of each element to the manager by increment an
 *        index that begin at 0 and end at (size-1).
 *
 * Show comments from PlyAbstractValuesSetter for more information.
 */
template<typename Manager>
class PlyDirectScalarValuesSetter : public PlyAbstractValuesSetter
{
public:
    typedef typename Manager::value_type value_type;

    /**
     * @brief Construct it
     * @param size : the total number of elements (lines). Per example for vertex the number of vertex.
     * @param manager : the manager that will receive values readed.
     */
    PlyDirectScalarValuesSetter(const size_t& size,
                          const Manager* manager) : PlyAbstractValuesSetter(size) {
        m_manager = const_cast<Manager*>(manager);
        m_currentIndex = 0;
    }

    void beginReadElement() {
        m_currentIndex = 0;
        m_manager->resize(m_size);
    }

    void setValue(const double& value, const int& channelIndex) {
        Q_UNUSED(channelIndex)
        m_scalarValue = value;
    }

    void oneLineOfElementReaded() {
        (*m_manager)[m_currentIndex] = m_scalarValue;
        ++m_currentIndex;
    }

private:
    size_t      m_currentIndex;
    Manager*    m_manager;
    value_type  m_scalarValue;
};

#endif // PLYDIRECTSCALARVALUESSETTER_H

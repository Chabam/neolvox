#ifndef PLYFILLVALUESSETTER_H
#define PLYFILLVALUESSETTER_H

#include "plyabstractvaluessetter.h"

#include <QString>

/**
 * @brief This class will set values of one element to the entire array of the manager (0 to (size-1)).
 *
 *  Use this class if a PlyElement contains only 1 element but the array in the manager contains many
 *  elements and you want to fill it with the single element readed.
 *
 *  Show comments from PlyAbstractValuesSetter for more information.
 */
template<typename Manager>
class PlyFillValuesSetter : public PlyAbstractValuesSetter
{
public:
    typedef typename Manager::value_type value_type;

    PlyFillValuesSetter(const size_t& size,
                        const Manager* manager) : PlyAbstractValuesSetter(size) {
        m_manager = const_cast<Manager*>(manager);
    }

    void beginReadElement() {
        m_manager->resize(m_size);
    }

    void setValue(const double& value, const int& channelIndex) {
        m_values[channelIndex] = value;
    }

    void endReadElement() {
        for(size_t i=0; i<m_size; ++i)
            (*m_manager)[i] = m_values;
    }

private:
    Manager*    m_manager;
    value_type  m_values;
};

#endif // PLYFILLVALUESSETTER_H

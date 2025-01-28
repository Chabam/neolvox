#ifndef PLYVALUESRECORDER_H
#define PLYVALUESRECORDER_H

#include "plyabstractvaluessetter.h"

#include <vector>

/**
 * @brief This class will only record all values readed.
 *
 * Show comments from PlyAbstractValuesSetter for more information.
 */
template<typename Collection>
class PlyValuesRecorder : public PlyAbstractValuesSetter
{
public:
    /**
     * @brief Construct it
     * @param size : the total number of elements (lines). Per example for index the number of index.
     */
    PlyValuesRecorder(const size_t& size) : PlyAbstractValuesSetter(size) {
        m_currentIndex = 0;
    }

    void beginReadElement() {
        m_currentIndex = 0;
        m_values.resize(m_size);
    }

    void setValue(const double& value, const int& channelIndex) {
        m_values[m_currentIndex][channelIndex] = value;
    }

    void oneLineOfElementReaded() {
        ++m_currentIndex;
    }

    const Collection& getValues() const {
        return m_values;
    }

private:
    size_t      m_currentIndex;
    Collection  m_values;
};

typedef PlyValuesRecorder< std::vector<size_t> > PlyIndexValuesRecorder;
typedef PlyValuesRecorder< std::vector<double> > PlyScalarValuesRecorder;

#endif // PLYVALUESRECORDER_H

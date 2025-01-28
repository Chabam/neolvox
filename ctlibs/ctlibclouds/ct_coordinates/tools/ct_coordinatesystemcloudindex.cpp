#include "ct_coordinatesystemcloudindex.h"

CT_CoordinateSystemCloudIndex::CT_CoordinateSystemCloudIndex()
{
    m_resizeNewValue = 0;
}

CT_CoordinateSystemCloudIndex::CT_CoordinateSystemCloudIndex(size_t size)
{
    m_resizeNewValue = 0;
    resize(size);
}

const GLuint &CT_CoordinateSystemCloudIndex::constValueAt(const size_t &index) const
{
    return m_vector[index];
}

GLuint &CT_CoordinateSystemCloudIndex::valueAt(const size_t &index)
{
    return m_vector[index];
}

void CT_CoordinateSystemCloudIndex::replaceValueAt(const size_t &index, const GLuint &newValue)
{
    m_vector[index] = newValue;
}

void CT_CoordinateSystemCloudIndex::decreaseAfterValue(const GLuint &value)
{
    std::vector<GLuint>::iterator it = m_vector.begin();
    std::vector<GLuint>::iterator end = m_vector.end();

    while(it != end) {
        if((*it) > value)
            (*it) -= 1;

        ++it;
    }
}

size_t CT_CoordinateSystemCloudIndex::size() const
{
    return m_vector.size();
}

size_t CT_CoordinateSystemCloudIndex::memoryUsed() const
{
    return size() * sizeof(std::vector<GLuint>::value_type);
}

CT_AbstractCloud* CT_CoordinateSystemCloudIndex::copy() const
{
    return new CT_CoordinateSystemCloudIndex();
}

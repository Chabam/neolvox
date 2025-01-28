#ifndef CT_COORDINATESYSTEMCLOUDINDEX_H
#define CT_COORDINATESYSTEMCLOUDINDEX_H

#include "ct_cloud/abstract/ct_abstractcloud.h"

#include <vector>

class CTLIBCLOUDS_EXPORT CT_CoordinateSystemCloudIndex : public CT_AbstractCloud
{
public:
    CT_CoordinateSystemCloudIndex();
    CT_CoordinateSystemCloudIndex(size_t size);

    /**
     * @brief Returns the value at "index"
     */
    const GLuint& constValueAt(const size_t &index) const;

    /**
     * @brief Returns a reference of the value at "index"
     */
    GLuint& valueAt(const size_t &index);

    /**
     * @brief Replace the value at "index" with the newValue
     */
    void replaceValueAt(const size_t &index, const GLuint &newValue);

    /**
     * @brief Decrease (-1) all values that was > at value 'value'
     */
    void decreaseAfterValue(const GLuint &value);

    /**
     * @brief Set the value to set when a resize is called
     */
    void setResizeNewValue(GLuint value) {
        m_resizeNewValue = value;
    }

    /**
     * @brief return the size of the cloud
     */
    size_t size() const;

    size_t memoryUsed() const;

    /**
     * @brief return a copy
     */
    CT_AbstractCloud* copy() const;

protected:

    void erase(const size_t &beginIndex, const size_t &sizes) {
        size_t endIndex = (beginIndex+sizes)-1;
        size_t cpySize = size()-(beginIndex+sizes);

        if(cpySize > 0)
        {
            GLuint *data = m_vector.data();

            GLuint *dst = data+beginIndex;
            GLuint *src = data+endIndex;

            memcpy(dst, src, sizeof(GLuint)*cpySize);
        }

        resize(size()-sizes);
    }


    void resize(const size_t &newSize) {
        m_vector.resize(newSize, m_resizeNewValue);
        m_vector.shrink_to_fit();
    }

    void reserve(const size_t &newSize) {
        m_vector.reserve(newSize);
    }

private:
    std::vector<GLuint> m_vector;
    GLuint              m_resizeNewValue;
};

#endif // CT_COORDINATESYSTEMCLOUDINDEX_H

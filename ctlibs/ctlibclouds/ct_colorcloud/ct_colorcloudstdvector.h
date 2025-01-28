#ifndef CT_COLORCLOUDSTDVECTOR_H
#define CT_COLORCLOUDSTDVECTOR_H

#include "ct_colorcloud/abstract/ct_abstractcolorcloud.h"
#include "ct_cloud/ct_standardcloudstdvectort.h"

/**
 * A color cloud that use a std::vector
 */
class CTLIBCLOUDS_EXPORT CT_ColorCloudStdVector : public CT_StandardCloudStdVectorT<CT_Color>, public CT_AbstractColorCloud
{
public:
    CT_ColorCloudStdVector();
    CT_ColorCloudStdVector(size_t size);

    /**
     * @brief Returns the number of colors
     */
    size_t size() const;

    /**
     * @brief Returns the memory used in bytes for all colors in this object
     */
    size_t memoryUsed() const;

    /**
     * @brief Add the normal color in parameter to the cloud
     */
    void addColor(const CT_Color &color);

    /**
     * @brief Add a default color to the cloud and return a reference to it
     */
    CT_Color& addColor();

    /**
     * @brief Returns a reference to the color at index 'index'
     */
    CT_Color& colorAt(const size_t &index);

    /**
     * @brief Returns a const reference to the color at index 'index'
     */
    const CT_Color& constColorAt(const size_t &index) const;

    /**
     * @brief Returns a reference to the color at index 'index'
     */
    CT_Color& operator[](const size_t &index);

    /**
     * @brief Returns a const reference to the color at index 'index'
     */
    const CT_Color& operator[](const size_t &index) const;

    /**
     * @brief Returns a copy of this cloud
     */
    CT_AbstractCloud* copy() const;

protected:
    /**
     * @brief delete all elements of the cloud between beginIndex and (beginIndex+size-1) included.
     */
    void erase(const size_t &beginIndex, const size_t &sizes);

    /**
     * @brief resize the collection
     */
    void resize(const size_t &newSize);

    /**
     * @brief reserve the collection
     */
    void reserve(const size_t &newSize);
};

#endif // CT_COLORCLOUDSTDVECTOR_H

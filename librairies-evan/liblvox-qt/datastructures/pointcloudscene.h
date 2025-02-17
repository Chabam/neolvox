#ifndef POINTCLOUDSCENE_H
#define POINTCLOUDSCENE_H

#include <Eigen/Geometry>

namespace lvox
{
class __declspec(dllexport) PointCloudScene
{
public:
    virtual int getSize() = 0;
    virtual Eigen::Vector3d getPointByIndex(size_t index) = 0;
    virtual void boundingBox(Eigen::Vector3d& _outSceneBoundingBoxMinCoordinates,
                             Eigen::Vector3d& _outSceneBoundingBoxMaxCoordinates) = 0;

    //struct Iterator
    //{
    //    using iterator_category = std::forward_iterator_tag;
    //    using difference_type   = std::ptrdiff_t;
    //    using value_type        = Eigen::Vector3d;
    //    using pointer           = Eigen::Vector3d*;  // or also value_type*
    //    using reference         = Eigen::Vector3d&;  // or also value_type&
    //    Iterator(pointer ptr) : m_ptr(ptr) {}

    //    reference operator*() const { return *m_ptr; }
    //    pointer operator->() { return m_ptr; }

    //    // Prefix increment
    //    Iterator& operator++() { m_ptr++; return *this; }

    //    // Postfix increment
    //    Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

    //    friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
    //    friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
    //private:

    //    pointer m_ptr;
    //};

    // virtual Iterator begin() = 0;
    // virtual Iterator end() = 0;
};
} // namespace lvox

#endif // POINTCLOUDSCENE_H

#ifndef CT_POINTITERATOR_H
#define CT_POINTITERATOR_H

#include "ct_defines.h"
#include "ct_point.h"

template<typename T> class CT_CloudIndexIteratorT;

class CT_AbstractCoordinateSystem;

/**
 * @brief Class to iterate over points
 */
class CTLIBCLOUDS_EXPORT CT_PointIterator
{
public:
    /**
     * @brief Create an iterator with a point cloud index
     */
    CT_PointIterator(const CT_AbstractPointCloudIndex *pci);

    /**
     * @brief Create an iterator with a cloud index registered. The constructor verify that the
     *        cloud index is a pointer to point cloud.
     */
    CT_PointIterator(CT_CIR cir);

    /**
     * @brief Copy constructor
     */
    CT_PointIterator(const CT_PointIterator& it);
    ~CT_PointIterator();

    /**
     * @brief Returns true if has a next point
     */
    bool hasNext() const;

    /**
     * @brief Go to next point
     */
    CT_PointIterator& next();

    /**
     * @brief Returns true if has a previous point
     */
    bool hasPrevious() const;

    /**
     * @brief Go to previous point
     */
    CT_PointIterator& previous();

    /**
     * @brief Jump from (current position) to (current position + n)
     * @warning call method "hasNext()" after this method
     */
    void jump(size_t n);

    /**
     * @brief Go to front of the list
     */
    void toFront();

    /**
     * @brief Go to end of the list
     */
    void toBack();

    /**
     * @brief Returns the size of the cloud index
     */
    size_t size() const;

    /**
     * @brief Return the current point (converted to double)
     */
    const CT_Point& currentPoint() const;
    inline const CT_Point& cT() const { return currentPoint(); }

    /**
     * @brief Return the globalIndex of the current point
     */
    size_t currentGlobalIndex() const;
    inline size_t cIndex() const { return currentGlobalIndex(); }

    /**
     * @brief Return the coordinate system of the current point
     */
    CT_AbstractCoordinateSystem* currentCoordinateSystem() const;

    /**
     * @brief Return the coordinate system index of the current point
     */
    GLuint currentCoordinateSystemIndex() const;

    /**
     * @brief Return a reference to the current point (internal point used in cloud)
     * @warning Not intended for direct use by plugin developper
     */
    CT_PointData& currentInternalPoint();

    /**
     * @brief Return a const reference to the current point (internal point used in cloud)
     * @warning Not intended for direct use by plugin developper
     */
    const CT_PointData& currentConstInternalPoint() const;

protected:

    CT_CloudIndexIteratorT<CT_PointData>    *m_it;
    mutable CT_Point                        m_p;

    /**
     * @brief Init the iterator with a point cloud index
     */
    void init(const CT_AbstractPointCloudIndex *pci);
};

#endif // CT_POINTITERATOR_H

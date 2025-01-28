#ifndef CT_MUTABLEPOINTITERATOR_H
#define CT_MUTABLEPOINTITERATOR_H

#include "ct_iterator/ct_pointiterator.h"

class CT_CoordinateSystemManager;

/**
 * @brief Class to iterate over points and modify it.
 * @warning Use this class only in a CT_ReaderXXX or when you create a cloud of points in your step !
 */
class CTLIBCLOUDS_EXPORT CT_MutablePointIterator
{
public:
    /**
     * @brief Create an iterator for a point cloud index
     */
    CT_MutablePointIterator(const CT_AbstractPointCloudIndex *pci);

    /**
     * @brief Create an iterator for a cloud index registered. The constructor verify that the
     *        cloud index is a pointer to point cloud.
     */
    CT_MutablePointIterator(CT_CIR cir);

    /**
     * @brief Copy constructor
     */
    CT_MutablePointIterator(const CT_MutablePointIterator &it);
    ~CT_MutablePointIterator();

    /**
     * @brief Returns true if has a next point
     */
    bool hasNext() const;

    /**
     * @brief Go to next point
     */
    CT_MutablePointIterator& next();

    /**
     * @brief Returns true if has a previous point
     */
    bool hasPrevious() const;

    /**
     * @brief Go to previous point
     */
    CT_MutablePointIterator& previous();

    /**
     * @brief Jump from (current position) to (current position + n)
     * @warning call method "hasNext()" after this method
     */
    CT_MutablePointIterator& jump(size_t n);

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

    // DEVELOPER POINT

    /**
     * @brief Return the current point (converted to double)
     */
    const CT_Point& currentPoint() const;
    inline const CT_Point& cT() const { return currentPoint(); }

    /**
     * @brief Replace the current point by a new point
     */
    void replaceCurrentPoint(const CT_Point &p);

    // INTERNAL POINT

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

    /**
     * @brief Replace the current point by a new point
     * @warning Not intended for direct use by plugin developper
     */
    void replaceCurrentInternalPoint(const CT_PointData &p);

    /**
     * @brief Replace the current point by a new point. You must pass the index of the coordinate system that will be set for this point.
     * @warning Be sure to pass a valid coordinate system index
     * @warning Not intended for direct use by plugin developper
     */
    void replaceCurrentInternalPoint(const CT_PointData &p, const GLuint &csIndex);

private:

    CT_PointIterator            *m_it;
    CT_CoordinateSystemManager  *m_csm;
};

#endif // CT_MUTABLEPOINTITERATOR_H

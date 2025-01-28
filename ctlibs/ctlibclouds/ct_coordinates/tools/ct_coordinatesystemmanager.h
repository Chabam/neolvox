#ifndef CT_COORDINATESYSTEMMANAGER_H
#define CT_COORDINATESYSTEMMANAGER_H

#include "ct_coordinates/abstract/ct_abstractcoordinatesystem.h"
#include "ct_coordinates/tools/ct_coordinatesystemcloudindex.h"
#include "ct_cloud/registered/ct_stdcloudregisteredt.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"
#include "ct_accessor/ct_pointaccessor.h"
#include "ct_cloud/tools/iglobalcloudlistener.h"

#include <QVector>
#include <QSharedPointer>

/**
 * @brief Manage coordinate system.
 */
class CTLIBCLOUDS_EXPORT CT_CoordinateSystemManager : public IGlobalCloudListener
{
public:
    CT_CoordinateSystemManager();
    ~CT_CoordinateSystemManager();

    /**
     * @brief Set the precision in mm. Per example 1 for 1 mm of precision, 0.1 for 0.1mm of precision, etc...
     * @return false if the global point cloud is not empty (you can change the precision if points are present), true otherwise.
     */
    bool setPrecision(const double &precision);

    /**
     * @brief Compute the coordinate system for point passed in parameter. If already not exist (used by another point) we will create it and add it to the collection.
     * @return Return the coordinate system that must be used to convert the point and set in parameter "csIndex" the index of the coordinate system.
     */
    CT_AbstractCoordinateSystem* computeCoordinateSystemForPointAndAddItToCollection(const CT_Point &p, GLuint &csIndex);

    /**
     * @brief Create the coordinate system for coordinates passed in parameter. Don't forget to delete the pointer if you don't use it later !
     */
    CT_AbstractCoordinateSystem* createCoordinateSystemForCoordinates(const double &x, const double &y, const double &z) const;

    /**
     * @brief Return the unique key that represent a coordinate system for coordinates passed in parameter
     */
    uint computeUniqueKeyForCoordinates(const double &x, const double &y, const double &z) const;

    /**
     * @brief Return the unique key that represent a coordinate system for coordinates passed in parameter and fill tab passed in parameter
     */
    uint computeUniqueKeyForCoordinatesAndFillTab(const double &x, const double &y, const double &z, int tab[3]) const;

    /**
     * @brief Compute the offset for specified tab (get it with method "computeUniqueKeyForCoordinatesAndFillTab")
     */
    void computeOffsetForTab(int tab[3], Eigen::Vector3d& offset) const;

    /**
     * @brief Returns the coordinate system for the point at index 'globalIndex' in the global points cloud
     */
    CT_AbstractCoordinateSystem* coordinateSystemForPointAt(const size_t &globalIndex) const;

    /**
     * @brief Returns the coordinate system index for the point at index 'globalIndex' in the global points cloud
     */
    GLuint coordinateSystemIndexForPointAt(const size_t &globalIndex) const;

    /**
     * @brief Change the index of the coordinate system of the point at index 'globalIndex' in the global points cloud
     */
    void setCoordinateSystemForPointAt(const size_t &globalIndex, const GLuint &coordinateSystemIndex) const;

    /**
     * @brief Returns the number of coordinate system installed. At least one coordinate system
     *        of type CT_DefaultCoordinateSystem is present in this manager.
     */
    int size() const;

    /**
     * @brief Returns the coordinate system at 'index'
     */
    CT_AbstractCoordinateSystem* coordinateSystemAt(const GLuint &csIndex) const;

    /**
     * @brief Return the index of the coordinate system passed in parameter
     */
    GLuint indexOfCoordinateSystem(const CT_AbstractCoordinateSystem *cs) const;

    /**
     * @brief Returns the index cloud that contains for each points the index of the coordinate system used. You can modify
     *        this cloud but be careful not to put an invalid value.
     *
     * @warning Don't add or remove index ! just replace index.
     */
    CT_CoordinateSystemCloudIndex* indexCloudOfCoordinateSystemOfPoints() const;

private:
    QSharedPointer< CT_StdCloudRegisteredT< CT_CoordinateSystemCloudIndex > >   m_indexOfCoordinateSystemOfPoints;      // cloud of index of coordinate system for all points in the global cloud

    QVector<uint>                                                               m_csKey;                                // key of coordinate system
    QVector<CT_AbstractCoordinateSystem*>                                       m_cs;                                   // list of coordinate system

    uint                                                                        m_lastCsKeyUsed;                        // last key used
    CT_AbstractCoordinateSystem*                                                m_lastCsUsed;                           // last coordinate system used
    GLuint                                                                      m_lastCsIndexUsed;                      // last coordinate system index used

    int                                                                         m_sizeOfIntTabInByte;                   // size in bytes of the tab used to compute index of point coordinate

    long double                                                                 m_csKeySum;                             // the variable to sum to coordinate of a points to compute it's key
    long double                                                                 m_csKeyDividor;                         // the variable to divide to coordinate of a points to compute it's key

    CT_PointAccessor                                                            m_pAccess;                              // an object to get the size of the global point cloud

    /**
     * @brief Init coordinate system
     */
    void init();

    /**
     * @brief Clear all coordinate system
     */
    void clear();

protected:

    /**
     * @brief Called when the global cloud is imputed of elements between beginIndex and endIndex (endIndex = beginIndex+size).
     */
    void cloudDeleted(const size_t &beginIndex, const size_t &size);

    /**
     * @brief Called when the size of the global cloud has increased
     */
    void cloudAdded(const size_t &size) { Q_UNUSED(size); }
};

#endif // CT_COORDINATESYSTEMMANAGER_H

#ifndef CT_ABSTRACTGEOMETRICALITEM_H
#define CT_ABSTRACTGEOMETRICALITEM_H

#include "ct_abstractsingularitemdrawable.h"

/**
 * @brief Represent an item that is a leaf of a group in the tree structure. Only singular
 *        item can have item attributes.
 *
 *        This item has a bounding box ! Singular item has only coordinates of the center.
 */
class CTLIBSTRUCTURE_EXPORT CT_AbstractGeometricalItem : public CT_AbstractSingularItemDrawable
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractGeometricalItem, CT_AbstractSingularItemDrawable, Geometrical Item)

    using SuperClass = CT_AbstractSingularItemDrawable;

public:
    CT_AbstractGeometricalItem();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - Unique ID
     *          - Pointer of base and alternative draw manager
     *          - Displayable name
     *          - Center coordinates
     *          - Default Color
     *          - Min and Max coordinates (bounding box)
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractGeometricalItem(const CT_AbstractGeometricalItem& other) = default;

    /**
     * @brief Set the x center coordinate of this item
     */
    virtual void setCenterX(double x);

    /**
     * @brief Set the y center coordinate of this item
     */
    virtual void setCenterY(double y);

    /**
     * @brief Set the z center coordinate of this item
     */
    virtual void setCenterZ(double z);

    /**
     * @brief Returns the x center coordinate of this item
     */
    double centerX() const override;

    /**
     * @brief Returns the y center coordinate of this item
     */
    double centerY() const override;

    /**
     * @brief Returns the z center coordinate of this item
     */
    double centerZ() const override;

    /**
     * @brief Set coordinate of the center
     */
    inline virtual void setCenterCoordinate(const Eigen::Vector3d& center) {m_centerCoordinates = center;}

    /**
     * @brief Returns coordinate of the center
     */
    inline const Eigen::Vector3d& centerCoordinate() const {return m_centerCoordinates;}

    /**
     * @brief Returns true if this item has a bounding box. All geometrical item has a bounding box so returns true by default.
     */
    bool hasBoundingBox() const override {return true;}

    /**
     * @brief Get the bounding box of the item
     */
    void boundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const override;

    /**
     * @brief Get the bounding box of the item but scaled
     *
     *        operation example ===> maxX = cx + (abs(maxX - cx) * xCoeff)
     *        operation example ===> minX = cx - (abs(minX - cx) * xCoeff)
     */
    void scaledBoundingBox(double xCoeff, double yCoeff, double zCoeff, Eigen::Vector3d &min, Eigen::Vector3d &max);

    /**
     * @brief Get the bounding box of the item but scaled
     *
     *        operation example ===> maxX = maxX + xMeters
     *        operation example ===> minX = minX - xMeters
     */
    void bufferedBoundingBox(double xMeters, double yMeters, double zMeters, Eigen::Vector3d &min, Eigen::Vector3d &max);

    /**
     * @brief Change the bounding box
     */
    void setBoundingBox(double minx, double miny, double minz, double maxx, double maxy, double maxz);

    /**
     * @brief Change the bounding box. Center coordinates will be modified according to this bounding box !
     */
    virtual void setBoundingBox(const Eigen::Vector3d& min, const Eigen::Vector3d& max);

    /**
     * @brief Change the center of the item from coordinates of the bounding box
     */
    void updateCenterFromBoundingBox();

    /**
     * @brief Get min X coordinate of the bounding box
     */
    inline double minX() const {return m_minCoordinates(0);}

    /**
     * @brief Get min Y coordinate of the bounding box
     */
    inline double minY() const {return m_minCoordinates(1);}

    /**
     * @brief Get min Z coordinate of the bounding box
     */
    inline double minZ() const {return m_minCoordinates(2);}

    /**
     * @brief Get max X coordinate of the bounding box
     */
    inline double maxX() const {return m_maxCoordinates(0);}

    /**
     * @brief Get max Y coordinate of the bounding box
     */
    inline double maxY() const {return m_maxCoordinates(1);}

    /**
     * @brief Get max Z coordinate of the bounding box
     */
    inline double maxZ() const {return m_maxCoordinates(2);}

    /**
     * @brief Get the size in X
     */
    inline double sizeX() const {return m_maxCoordinates(0) - m_minCoordinates(0);}

    /**
     * @brief Get the size in Y
     */
    inline double sizeY() const {return m_maxCoordinates(1) - m_minCoordinates(1);}

    /**
     * @brief Get the size in Z
     */
    inline double sizeZ() const {return m_maxCoordinates(2) - m_minCoordinates(2);}

    /**
     * @brief Returns min coordinate of bounding box
     */
    Eigen::Vector3d minCoordinates() const;

    /**
     * @brief Returns max coordinate of bounding box
     */
    Eigen::Vector3d maxCoordinates() const;

private:
    /**
     * @brief Minimum corner of the bounding box
     */
    Eigen::Vector3d     m_minCoordinates;

    /**
     * @brief Maximum corner of the bounding box
     */
    Eigen::Vector3d     m_maxCoordinates;

    /**
     * @brief Coordinates of the center of this item
     */
    Eigen::Vector3d     m_centerCoordinates;

    CT_DEFAULT_IA_BEGIN(CT_AbstractGeometricalItem)
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataCx(), &CT_AbstractGeometricalItem::centerX, QObject::tr("Center X"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataCy(), &CT_AbstractGeometricalItem::centerY, QObject::tr("Center Y"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataCz(), &CT_AbstractGeometricalItem::centerZ, QObject::tr("Center Z"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataX(), &CT_AbstractGeometricalItem::minX, QObject::tr("Min X"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataX(), &CT_AbstractGeometricalItem::maxX, QObject::tr("Max X"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataY(), &CT_AbstractGeometricalItem::minY, QObject::tr("Min Y"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataY(), &CT_AbstractGeometricalItem::maxY, QObject::tr("Max Y"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataZ(), &CT_AbstractGeometricalItem::minZ, QObject::tr("Min Z"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataZ(), &CT_AbstractGeometricalItem::maxZ, QObject::tr("Max Z"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataXDimension(), &CT_AbstractGeometricalItem::sizeX, QObject::tr("Size X"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataYDimension(), &CT_AbstractGeometricalItem::sizeY, QObject::tr("Size Y"))
    CT_DEFAULT_IA_V2(CT_AbstractGeometricalItem, CT_AbstractCategory::staticInitDataZDimension(), &CT_AbstractGeometricalItem::sizeZ, QObject::tr("Size Z"))
    CT_DEFAULT_IA_END(CT_AbstractGeometricalItem)
};

#endif // CT_ABSTRACTGEOMETRICALITEM_H

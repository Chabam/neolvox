#ifndef CT_ABSTRACTPOINTATTRIBUTESSCALAR_H
#define CT_ABSTRACTPOINTATTRIBUTESSCALAR_H

#include "ct_itemdrawable/abstract/ct_abstractpointsattributes.h"
#include "ct_attributes/abstract/ct_abstractattributesscalar.h"

/**
 * @brief Represents a cloud of attributes of scalar type (int, float, double, etc...) for points.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractPointAttributesScalar : public CT_AbstractPointsAttributes, public CT_AbstractAttributesScalar
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractPointAttributesScalar, CT_AbstractPointsAttributes, Point attributes)

public:
    CT_AbstractPointAttributesScalar() = default;

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
     *          - Shared pointer to point cloud index registered
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractPointAttributesScalar(const CT_AbstractPointAttributesScalar& other) = default;

    /**
     * @brief Returns true if the value at the specified global index has been set in the total attribute cloud, false otherwise
     */
    virtual bool hasBeenSet(const size_t& globalIndex) const override = 0;

    /**
     * @brief Returns the number of values set in the total attribute cloud
     */
    virtual size_t numberOfSetValues() const override = 0;

    /**
     * @brief Returns true if at least one value has been set in the total attribute cloud
     */
    virtual bool hasValues() const override = 0;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to count how many values has been set.
     */
    virtual size_t numberOfSetLocalValues() const override = 0;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to returns true if at least one value has been set.
     */
    virtual bool hasLocalValues() const override = 0;
};

#endif // CT_ABSTRACTPOINTATTRIBUTESSCALAR_H

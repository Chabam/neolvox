#ifndef CT_ABSTRACTATTRIBUTES_H
#define CT_ABSTRACTATTRIBUTES_H

#include "ctlibstructureaddon_global.h"

#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"
#include "ct_defines.h"

/**
 * @brief Represents a cloud of attributes likes colors or values. He is a singular item a bit special because
 *        he will be used by a graphics view to display colors or normals of points, edges or faces.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractAttributes : public CT_AbstractSingularItemDrawable
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractAttributes, CT_AbstractSingularItemDrawable, Attributes)

    using SuperClass = CT_AbstractSingularItemDrawable;

public:
    CT_AbstractAttributes();

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
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractAttributes(const CT_AbstractAttributes& other) = default;

    /**
     * @brief Returns true if the value at the specified global index has been set in the total attribute cloud, false otherwise
     */
    virtual bool hasBeenSet(const size_t& globalIndex) const = 0;

    /**
     * @brief Returns the number of values set in the total attribute cloud
     */
    virtual size_t numberOfSetValues() const = 0;

    /**
     * @brief Returns true if at least one value has been set in the total attribute cloud
     */
    virtual bool hasValues() const = 0;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to count how many values has been set.
     */
    virtual size_t numberOfSetLocalValues() const = 0;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to returns true if at least one value has been set.
     */
    virtual bool hasLocalValues() const = 0;

    /**
     * @brief Copy attributes of the source cloud for the destination cloud
     * @param source : the source cloud
     * @param destination : the destination cloud (must be the same size as the source cloud)
     * @return false if destination has values already set ! true otherwise
     */
    virtual bool copyAttributesOfSForD(CT_CIR source, CT_CIR destination) = 0;
};

#endif // CT_ABSTRACTATTRIBUTES_H

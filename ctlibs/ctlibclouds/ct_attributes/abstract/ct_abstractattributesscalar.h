#ifndef CT_ABSTRACTATTRIBUTESSCALAR_H
#define CT_ABSTRACTATTRIBUTESSCALAR_H

#include <functional>

#include "ctlibclouds_global.h"

/**
 * @brief A cloud attributes of type Scalar (float, int, double, etc..). The min() and max() values are used to define the upper
 *        and lower gradient color to apply to the object to colorize per example.
 */
class CTLIBCLOUDS_EXPORT CT_AbstractAttributesScalar
{
public:
    /**
     * @brief The visitor receive the global index of the scalar and the scalar as double. The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using DVisitor = std::function<bool (const size_t& /*globalIndex*/, double /*value*/)>;

    /**
     * @brief The visitor receive the global index of the scalar. The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using IVisitor = std::function<bool (const size_t& /*globalIndex*/)>;

    CT_AbstractAttributesScalar();
    CT_AbstractAttributesScalar(const CT_AbstractAttributesScalar& other);
    CT_AbstractAttributesScalar& operator=(const CT_AbstractAttributesScalar& other);

    virtual ~CT_AbstractAttributesScalar() = default;

    // GLOBAL //
    /**
     * @brief Returns the scalar at the specified global index
     */
    virtual double scalarAsDoubleAt(const size_t& globalIndex, bool* hasBeenSet = nullptr) const = 0;

    /**
     * @brief Returns true if the scalar at the specified global index has been set
     */
    virtual bool hasBeenSet(const size_t& globalIndex) const = 0;

    /**
     * @brief Returns the number of set scalars in the total scalar cloud
     */
    virtual size_t numberOfSetValues() const = 0;

    /**
     * @brief Visit all defined scalars of the total scalar cloud
     */
    virtual bool visitValuesAsDouble(DVisitor v) const = 0;

    /**
     * @brief Visit all indexes of defined scalars of the total scalar cloud
     */
    virtual bool visitAllIndexesSet(IVisitor v) const = 0;

    /**
     * @brief Returns true if at least one scalar has been set in the total scalar cloud
     */
    virtual bool hasValues() const = 0;

    /**
     * @brief Returns the minimum value for the total scalar cloud
     */
    virtual double minScalarAsDouble() const = 0;

    /**
     * @brief Returns the maximum value for the total scalar cloud
     */
    virtual double maxScalarAsDouble() const = 0;

    // LOCAL //
    /**
     * @brief Returns the scalar at the specified local index. For performance prefer to use the method "visitLocalValues(...)"
     * @param localIndex : the local index, must be in range [0;numberOfSetLocalValues()[
     */
    virtual double scalarAsDoubleAtLocalIndex(const size_t& localIndex) const = 0;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to count how many scalars has been set.
     */
    virtual size_t numberOfSetLocalValues() const = 0;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to returns true if at least one scalar has been set.
     */
    virtual bool hasLocalValues() const = 0;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        and call the visitor each time a scalar has been set.
     * @param v : a function that will receive for first parameter the global index of the scalar and for second parameter the value of the scalar. The
     *            visitor must returns false if the visit must be aborted or true to continue the visit.
     * @return False if the visitor has aborted the visit, true otherwise.
     */
    virtual bool visitLocalValuesAsDouble(DVisitor v) const = 0;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        and call the visitor each time a scalar has been set.
     * @param v : a function that will receive the global index of the scalar. The
     *            visitor must returns false if the visit must be aborted or true to continue the visit.
     * @return False if the visitor has aborted the visit, true otherwise.
     */
    virtual bool visitLocalIndexesSet(IVisitor v) const = 0;

    /**
     * @brief Returns the minimum value for the local scalar cloud
     */
    virtual double minLocalScalarAsDouble() const = 0;

    /**
     * @brief Returns the maximum value for the local scalar cloud
     */
    virtual double maxLocalScalarAsDouble() const = 0;
};

#endif // CT_ABSTRACTATTRIBUTESSCALAR_H

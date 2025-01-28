#ifndef CT_ATTRIBUTESSCALART_H
#define CT_ATTRIBUTESSCALART_H

#include "ct_attributes/managers/abstract/ct_abstractxattributemanager.h"
#include "ct_attributes/managers/ct_scalarminmaxmanager.h"
#include "ct_cloudindex/abstract/ct_abstractcloudindex.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregistered.h"

/**
 *  Attributes of scalar type (int, float, double, etc...)
 */
template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR = SCALAR>
class CT_AttributesScalarT : public InheritFrom
{
public:
    /**
     * @brief The visitor receive the global index of the scalar and the scalar as double. The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using DVisitor = std::function<bool (const size_t& /*globalIndex*/, double /*value*/)>;

    /**
     * @brief The visitor receive the global index of the attribute and the attribute (SCALAR). The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using SVisitor = std::function<bool (const size_t& /*globalIndex*/, SCALAR /*value*/)>;

    /**
     * @brief The visitor receive the global index of the attribute. The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using IVisitor = typename CT_AbstractXAttributeManager<MANAGER_SCALAR>::IVisitor;

    CT_AttributesScalarT();

    /**
     * @brief Create a collection of SCALAR and use the specified min and max value.
     */
    CT_AttributesScalarT(CT_CIR cir,
                         CT_AbstractXAttributeManager<MANAGER_SCALAR>& manager,
                         const SCALAR& min,
                         const SCALAR& max);

    /**
     * @brief Create a collection of SCALAR and compute the local min and max value.
     */
    CT_AttributesScalarT(CT_CIR cir,
                         CT_AbstractXAttributeManager<MANAGER_SCALAR>& manager);
    ~CT_AttributesScalarT();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Attributes manager pointer
     *          - Min and max value are copied
     *
     *        What is initialized differently :
     */
    CT_AttributesScalarT(const CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>& other) = default;
    CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>& operator=(const CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>& other) = default;

    // GLOBAL //
    /**
     * @brief Returns the scalar converted in double at the specified global index
     */
    double scalarAsDoubleAt(const size_t& globalIndex, bool* hasBeenSet = nullptr) const;

    /**
     * @brief Returns the scalar converted in double at the specified global index
     */
    SCALAR scalarAt(const size_t& globalIndex, bool* hasBeenSet = nullptr) const;

    /**
     * @brief Returns true if the scalar at the specified global index has been set
     */
    bool hasBeenSet(const size_t& globalIndex) const;

    /**
     * @brief Returns the number of set scalars in the total scalar cloud
     */
    size_t numberOfSetValues() const;

    /**
     * @brief Visit all defined scalars of the total scalar cloud
     */
    bool visitValuesAsDouble(DVisitor v) const;

    /**
     * @brief Visit all indexes of defined scalars of the total scalar cloud
     */
    bool visitAllIndexesSet(IVisitor v) const;

    /**
     * @brief Returns true if at least one scalar has been set in the total scalar cloud
     */
    bool hasValues() const;

    /**
     * @brief Returns the minimum scalar value of the total scalar cloud converted in double
     */
    double minScalarAsDouble() const;

    /**
     * @brief Returns the maximum scalar value of the total scalar cloud converted in double
     */
    double maxScalarAsDouble() const;

    /**
     * @brief Returns the minimum scalar of the total scalar cloud
     */
    SCALAR minScalar() const;

    /**
     * @brief Returns the maximum scalar of the total scalar cloud
     */
    SCALAR maxScalar() const;

    /**
     * @brief Visit all defined scalars of the total scalar cloud
     */
    bool visitValues(SVisitor v) const;

    // LOCAL //
    /**
     * @brief Returns the scalar converted in double at the specified local index. For performance prefer to use the method "visitLocalValues(...)"
     * @param localIndex : the local index, must be in range [0;numberOfSetLocalValues()[
     */
    double scalarAsDoubleAtLocalIndex(const size_t& localIndex) const;

    /**
     * @brief Returns the scalar at the specified local index. For performance prefer to use the method "visitLocalValues(...)"
     * @param localIndex : the local index, must be in range [0;numberOfSetLocalValues()[
     */
    SCALAR scalarAtLocalIndex(const size_t& localIndex) const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to count how many scalars has been set.
     */
    size_t numberOfSetLocalValues() const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to returns true if at least one scalar has been set.
     */
    bool hasLocalValues() const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        and call the visitor each time a scalar has been set.
     * @param v : a function that will receive for first parameter the global index of the scalar and for second parameter the value of the scalar. The
     *            visitor must returns false if the visit must be aborted or true to continue the visit.
     * @return False if the visitor has aborted the visit, true otherwise.
     */
    bool visitLocalValuesAsDouble(DVisitor v) const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        and call the visitor each time a scalar has been set.
     * @param v : a function that will receive the global index of the scalar. The
     *            visitor must returns false if the visit must be aborted or true to continue the visit.
     * @return False if the visitor has aborted the visit, true otherwise.
     */
    bool visitLocalIndexesSet(IVisitor v) const;

    /**
     * @brief Returns the minimum scalar converted in double of the local scalar cloud
     */
    double minLocalScalarAsDouble() const;

    /**
     * @brief Returns the maximum scalar converted in double of the local scalar cloud
     */
    double maxLocalScalarAsDouble() const;

    /**
     * @brief Returns the minimum scalar of the local scalar cloud
     */
    SCALAR minLocalScalar() const;

    /**
     * @brief Returns the maximum scalar of the local scalar cloud
     */
    SCALAR maxLocalScalar() const;

    /**
     * @brief Modify the local min and max value
     */
    void setLocalMinMax(const SCALAR& min, const SCALAR& max);

    /**
     * @brief Visit all defined scalars of the total scalar cloud
     */
    bool visitLocalValues(SVisitor v) const;

    /**
     * @brief Returns the manager of the total scalar cloud
     */
    CT_AbstractXAttributeManager<MANAGER_SCALAR>* scalarsManager() const;

    /**
     * @brief Returns the value of the mask used
     */
    virtual quint64 mask() const { return 0xFFFFFFFF; }

    /**
     * @brief Copy scalars of the source cloud for the destination cloud and let a modificator modify the new value to set
     * @param source : the source cloud
     * @param destination : the destination cloud (must be the same size as the source cloud)
     * @param modificator : a function that must modify the new value to set or nullptr to copy it only
     * @return false if destination has values already set ! true otherwise
     */
    bool copyAndModifyAttributesOfSForD(CT_CIR source, CT_CIR destination, typename CT_AbstractXAttributeManager<MANAGER_SCALAR>::AttributeModificator modificator = nullptr) { return scalarsManager()->copyAndModifyAttributesOfSForD(source, destination, modificator); }

private:
    CT_CIR                                          m_cir;
    SCALAR                                          m_min;
    SCALAR                                          m_max;
    CT_AbstractXAttributeManager<MANAGER_SCALAR>*   m_manager;

protected:
    virtual SCALAR convertScalarOfManagerToScalar(const MANAGER_SCALAR& scalar) const
    {
        return internalConvertScalarOfManagerToScalar(scalar, std::integral_constant<bool, std::is_convertible<MANAGER_SCALAR, SCALAR>::value>());
    }

    SCALAR internalConvertScalarOfManagerToScalar(const MANAGER_SCALAR& scalar, std::true_type) const
    {
        return scalar;
    }

    SCALAR internalConvertScalarOfManagerToScalar(const MANAGER_SCALAR&, std::false_type) const
    {
        return SCALAR();
    }
};

#include "ct_attributes/ct_attributesscalart.hpp"

#endif // CT_ATTRIBUTESSCALART_H

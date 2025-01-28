#ifndef CT_ABSTRACTXATTRIBUTEMANAGER_H
#define CT_ABSTRACTXATTRIBUTEMANAGER_H

#include <functional>
#include <QVector>

#include "ct_point.h"
#include "ct_mesh/ct_edge.h"
#include "ct_mesh/ct_face.h"

// You need void_t to avoid a warning about the lhs of the comma operator
// having no effect. C++ 17 has std::void_t
template<class...> using void_t = void;

/**
 * @brief SFINAE tool to check if the cloud manager is a point cloud manager. If the enum "APPLICABLE_TO_POINT" is present
 *        we deduce that it is a point cloud manager.
 */
#define IsAPointCloudManager(ManagerT) std::is_same<typename ManagerT::ApplicableTo, CT_PointData>

/**
 * @brief SFINAE tool to check if the cloud manager is a edge cloud manager. If the enum "APPLICABLE_TO_EDGE" is present
 *        we deduce that it is a edge cloud manager.
 */
#define IsAEdgeCloudManager(ManagerT) std::is_same<typename ManagerT::ApplicableTo, CT_Edge>

/**
 * @brief SFINAE tool to check if the cloud manager is a face cloud manager. If the enum "APPLICABLE_TO_FACE" is present
 *        we deduce that it is a face cloud manager.
 */
#define IsAFaceCloudManager(ManagerT) std::is_same<typename ManagerT::ApplicableTo, CT_Face>

/**
 * @brief The CT_AbstractXAttributeManager class is the interface of
 *        manager that can create, access, and let the user of the class set
 *        attributes (of type T) for cloud that contains points,
 *        edges or faces.
 *
 *        The interface don't declare methods to create or set attributes
 *        because you must use the right class that can do that. This interface
 *        let the user access to attributes in read only mode.
 */
template<typename T>
class CT_AbstractXAttributeManager
{
public:
    /**
     * @brief The visitor receive the global index of the attribute and the attribute (T). The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using Visitor = std::function<bool (const size_t& /*globalIndex*/, const T& /*value*/)>;

    /**
     * @brief The visitor receive the global index of the attribute. The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using IVisitor = std::function<bool (const size_t& /*globalIndex*/)>;

    /**
     * @brief The modificator receive the element T to evaluate and must assign the new value to set in the other element T. So the modificator
     *        must returns an integer >0 to set the new value, 0 to ignore the new value and not set it or <0 to cancel the visit.
     */
    using AttributeModificator = std::function<int (const size_t& /*sourceGlobalIndex*/, const T& /*sourceValue*/, const size_t& /*destinationGlobalIndex*/, T& /*destinationValue*/)>;


    CT_AbstractXAttributeManager() = default;
    CT_AbstractXAttributeManager(const CT_AbstractXAttributeManager& other) = delete;
    CT_AbstractXAttributeManager& operator=(const CT_AbstractXAttributeManager& other) = delete;

    virtual ~CT_AbstractXAttributeManager() = default;

    /**
     * @brief Returns true if an attribute has been set at global index specified
     * @param globalIndex : the global index of the attribute
     */
    virtual bool hasBeenSet(const size_t& globalIndex) const = 0;

    /**
     * @brief Visit only attributes that has been set
     * @param v : the visitor
     * @return Returns true if the visitor has always returned true or if the collection is empty. False if the
     *         visitor has returned false.
     */
    virtual bool visitValues(Visitor v) const = 0;

    /**
     * @brief Visit only indexes that has attributes set
     * @param v : the visitor
     * @return Returns true if the visitor has always returned true or if the collection is empty. False if the
     *         visitor has returned false.
     */
    virtual bool visitAllIndexesSet(IVisitor v) const = 0;

    /**
     * @brief Returns the number of attributes that has values (hasBeenSet(...) == true). Be careful that you cannot
     *        increment a local index between 0 and size() to call "tAt" or "hasBeenSet". Prefer to
     *        use method "visitValues" or "tAtLocalIndex".
     */
    virtual size_t numberOfSetValues() const = 0;

    /**
     * @brief Returns false if at least one attribute has been set
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief Returns the attribute at the specified global index
     * @param globalIndex : the global index of the attributes.
     * @param hasBeenSet : if not nullptr will contains true if the attribute has been set, false otherwise (if it is an incorrect value)
     */
    virtual const T& tAt(const size_t& globalIndex, bool* hasBeenSet = nullptr) const = 0;

    /**
     * @brief Returns the attribute at the specified local index. The local index must be in range [0;size()[. Only values with hasBeenSet(...) == true
     *        are considered
     * @param localIndex : the local index of the attributes.
     */
    virtual const T& tAtLocalIndex(const size_t& localIndex) const = 0;

    /**
     * @brief Copy attributes of the source cloud for the destination cloud. If a modificator is set it will used before copy the value otherwise
     *        the value is just copied.
     * @param source : the source cloud
     * @param destination : the destination cloud (must be the same size as the source cloud)
     * @param modificator : the modificator to use to get the new value
     * @return false if destination has values already set ! true otherwise
     */
    virtual bool copyAndModifyAttributesOfSForD(CT_CIR source, CT_CIR destination, AttributeModificator modificator = nullptr) = 0;
};

/**
 * @brief SFINAE tool to check if the type is a base of CT_AbstractXAttributeManager<ATT>.
 */
template <typename T, typename ATT, typename = void>
struct IsABaseOfCT_AbstractXAttributeManager
    : std::false_type
{};

template <typename T, typename ATT>
struct IsABaseOfCT_AbstractXAttributeManager<T, ATT,
        typename std::enable_if<std::is_base_of<CT_AbstractXAttributeManager<ATT>, T>::value>::type>
    : std::true_type
{};

#endif // CT_ABSTRACTXATTRIBUTEMANAGER_H

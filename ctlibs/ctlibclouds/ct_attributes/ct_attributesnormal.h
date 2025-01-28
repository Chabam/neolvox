#ifndef CT_ATTRIBUTESNORMAL_H
#define CT_ATTRIBUTESNORMAL_H

#include "ct_normalcloud/abstract/ct_abstractnormalcloud.h"
#include "ct_attributes/managers/abstract/ct_abstractxattributemanager.h"
#include "ct_cloudindex/abstract/ct_abstractcloudindex.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregistered.h"

/**
 * @brief A cloud attributes of type "normals". Normals are applied directly to the object where draw normals.
 */
template<typename InheritFrom>
class CT_AttributesNormal : public InheritFrom
{
public:
    /**
     * @brief The visitor receive the global index of the attribute and the attribute (CT_Normal). The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using NormalVisitor = typename CT_AbstractXAttributeManager<CT_Normal>::Visitor;

    /**
     * @brief The visitor receive the global index of the attribute. The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using IVisitor = typename CT_AbstractXAttributeManager<CT_Normal>::IVisitor;

    CT_AttributesNormal();
    CT_AttributesNormal(CT_CIR cir, CT_AbstractXAttributeManager<CT_Normal>& manager);

    /**
     * @brief Copy constructor
     *
     *        What is copied :
     *          - Attributes manager pointer
     *
     *        What is initialized differently :
     */
    CT_AttributesNormal(const CT_AttributesNormal& other) = default;
    CT_AttributesNormal& operator=(const CT_AttributesNormal& other) = default;

    virtual ~CT_AttributesNormal() {}

    // GLOBAL //
    /**
     * @brief Returns the normal at the specified global index
     */
    const CT_Normal& constNormalAt(const size_t& globalIndex, bool* hasBeenSet = nullptr) const;

    /**
     * @brief Returns true if the normal at the specified global index has been set
     */
    bool hasBeenSet(const size_t& globalIndex) const;

    /**
     * @brief Returns the number of set normals in the total normal cloud
     */
    size_t numberOfSetValues() const;

    /**
     * @brief Visit all defined normals of the total normal cloud
     */
    bool visitValues(NormalVisitor v) const;

    /**
     * @brief Visit all indexes of defined normals of the total normal cloud
     */
    bool visitAllIndexesSet(IVisitor v) const;

    /**
     * @brief Returns true if at least one normal has been set in the total normal cloud
     */
    bool hasValues() const;

    // LOCAL //
    /**
     * @brief Returns the normal at the specified local index. For performance prefer to use the method "visitLocalValues(...)"
     * @param localIndex : the local index, must be in range [0;numberOfSetLocalValues()[
     */
    const CT_Normal& constNormalAtLocalIndex(const size_t& localIndex) const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to count how many normals has been set.
     */
    size_t numberOfSetLocalValues() const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to returns true if at least one normal has been set.
     */
    bool hasLocalValues() const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        and call the visitor each time a normal has been set.
     * @param v : a function that will receive for first parameter the global index of the normal and for second parameter the value of the normal. The
     *            visitor must returns false if the visit must be aborted or true to continue the visit.
     * @return False if the visitor has aborted the visit, true otherwise.
     */
    bool visitLocalValues(NormalVisitor v) const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        and call the visitor each time a normal has been set.
     * @param v : a function that will receive the global index of the normal. The
     *            visitor must returns false if the visit must be aborted or true to continue the visit.
     * @return False if the visitor has aborted the visit, true otherwise.
     */
    bool visitLocalIndexesSet(IVisitor v) const;

    /**
     * @brief Returns the manager of the total normal cloud
     */
    CT_AbstractXAttributeManager<CT_Normal>* normalsManager() const;

    /**
     * @brief Copy normals of the source cloud for the destination cloud and let a modificator modify the new value to set
     * @param source : the source cloud
     * @param destination : the destination cloud (must be the same size as the source cloud)
     * @param modificator : a function that must modify the new value to set or nullptr to copy it only
     * @return false if destination has values already set ! true otherwise
     */
    bool copyAndModifyAttributesOfSForD(CT_CIR source, CT_CIR destination, CT_AbstractXAttributeManager<CT_Normal>::AttributeModificator modificator = nullptr) { return normalsManager()->copyAndModifyAttributesOfSForD(source, destination, modificator); }

private:
    CT_CIR                                      m_cir;
    CT_AbstractXAttributeManager<CT_Normal>*    m_manager;
};


template<typename InheritFrom>
CT_AttributesNormal<InheritFrom>::CT_AttributesNormal() :
    m_manager(nullptr)
{
}

template<typename InheritFrom>
CT_AttributesNormal<InheritFrom>::CT_AttributesNormal(CT_CIR cir, CT_AbstractXAttributeManager<CT_Normal>& manager) :
    m_cir(cir),
    m_manager(&manager)
{
}

template<typename InheritFrom>
const CT_Normal& CT_AttributesNormal<InheritFrom>::constNormalAt(const size_t& globalIndex, bool* hasBeenSet) const
{
    return m_manager->tAt(globalIndex, hasBeenSet);
}

template<typename InheritFrom>
bool CT_AttributesNormal<InheritFrom>::hasBeenSet(const size_t& globalIndex) const
{
    return m_manager->hasBeenSet(globalIndex);
}

template<typename InheritFrom>
size_t CT_AttributesNormal<InheritFrom>::numberOfSetValues() const
{
    return m_manager->numberOfSetValues();
}

template<typename InheritFrom>
bool CT_AttributesNormal<InheritFrom>::visitValues(CT_AttributesNormal<InheritFrom>::NormalVisitor v) const
{
    return m_manager->visitValues(v);
}

template<typename InheritFrom>
bool CT_AttributesNormal<InheritFrom>::visitAllIndexesSet(IVisitor v) const
{
    return m_manager->visitAllIndexesSet(v);
}

template<typename InheritFrom>
bool CT_AttributesNormal<InheritFrom>::hasValues() const
{
    return !m_manager->isEmpty();
}

template<typename InheritFrom>
const CT_Normal& CT_AttributesNormal<InheritFrom>::constNormalAtLocalIndex(const size_t& localIndex) const
{
    size_t count = 0;
    const CT_Normal* normal = nullptr;

    visitValues([&count, &localIndex, &normal](const size_t&, const CT_Normal& value) -> bool
    {
        if(count == localIndex)
        {
            normal = &value;
            return false;
        }

        ++count;
        return true;
    });

    return *normal;
}

template<typename InheritFrom>
size_t CT_AttributesNormal<InheritFrom>::numberOfSetLocalValues() const
{
    CT_AbstractCloudIndex* ci = m_cir->abstractCloudIndex();

    const size_t size = ci->size();
    size_t finalSize = 0;

    for(size_t i=0; i<size; ++i)
    {
        if(m_manager->hasBeenSet(ci->indexAt(i)))
            ++finalSize;
    }

    return finalSize;
}

template<typename InheritFrom>
bool CT_AttributesNormal<InheritFrom>::hasLocalValues() const
{
    CT_AbstractCloudIndex* ci = m_cir->abstractCloudIndex();

    const size_t size = ci->size();

    for(size_t i=0; i<size; ++i)
    {
        if(m_manager->hasBeenSet(ci->indexAt(i)))
            return true;
    }

    return false;
}

template<typename InheritFrom>
bool CT_AttributesNormal<InheritFrom>::visitLocalValues(CT_AttributesNormal<InheritFrom>::NormalVisitor v) const
{
    CT_AbstractCloudIndex* ci = m_cir->abstractCloudIndex();

    const size_t size = ci->size();
    bool hasBeenSet = false;

    for(size_t i=0; i<size; ++i)
    {
        const size_t globalIndex = ci->indexAt(i);
        const CT_Normal& value = m_manager->tAt(globalIndex, &hasBeenSet);

        if(hasBeenSet)
        {
            if(!v(globalIndex, value))
                return false;
        }
    }

    return true;
}

template<typename InheritFrom>
bool CT_AttributesNormal<InheritFrom>::visitLocalIndexesSet(IVisitor v) const
{
    CT_AbstractCloudIndex* ci = m_cir->abstractCloudIndex();

    const size_t size = ci->size();

    for(size_t i=0; i<size; ++i)
    {
        const size_t globalIndex = ci->indexAt(i);

        if(m_manager->hasBeenSet(globalIndex) && !v(globalIndex))
                return false;
    }

    return true;
}

template<typename InheritFrom>
CT_AbstractXAttributeManager<CT_Normal>* CT_AttributesNormal<InheritFrom>::normalsManager() const
{
    return m_manager;
}


#endif // CT_ATTRIBUTESNORMAL_H

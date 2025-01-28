#ifndef CT_ATTRIBUTESCOLOR_H
#define CT_ATTRIBUTESCOLOR_H

#include "ct_colorcloud/abstract/ct_abstractcolorcloud.h"
#include "ct_attributes/managers/abstract/ct_abstractxattributemanager.h"
#include "ct_cloudindex/abstract/ct_abstractcloudindex.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregistered.h"

/**
 * @brief A cloud attributes of type "color". Colors are applied directly to the object to colorize it.
 */
template<typename InheritFrom>
class CT_AttributesColor : public InheritFrom
{
public:
    /**
     * @brief The visitor receive the global index of the attribute and the attribute (CT_Color). The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using ColorVisitor = typename CT_AbstractXAttributeManager<CT_Color>::Visitor;

    /**
     * @brief The visitor receive the global index of the attribute. The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using IVisitor = typename CT_AbstractXAttributeManager<CT_Color>::IVisitor;

    CT_AttributesColor();
    CT_AttributesColor(CT_CIR cir, CT_AbstractXAttributeManager<CT_Color>& manager);

    /**
     * @brief Copy constructor
     *
     *        What is copied :
     *          - Attributes manager pointer
     *
     *        What is initialized differently :
     */
    CT_AttributesColor(const CT_AttributesColor& other) = default;

    CT_AttributesColor& operator=(const CT_AttributesColor& other) = default;

    virtual ~CT_AttributesColor() {}

    // GLOBAL //
    /**
     * @brief Returns the color at the specified global index
     */
    const CT_Color& constColorAt(const size_t& globalIndex, bool* hasBeenSet = nullptr) const;

    /**
     * @brief Returns true if the color at the specified global index has been set
     */
    bool hasBeenSet(const size_t& globalIndex) const;

    /**
     * @brief Returns the number of set colors in the total color cloud
     */
    size_t numberOfSetValues() const;

    /**
     * @brief Visit all defined colors of the total color cloud
     */
    bool visitValues(ColorVisitor v) const;

    /**
     * @brief Visit all indexes of defined colors of the total color cloud
     */
    bool visitAllIndexesSet(IVisitor v) const;

    /**
     * @brief Returns true if at least one color has been set in the total color cloud
     */
    bool hasValues() const;

    // LOCAL //
    /**
     * @brief Returns the color at the specified local index. For performance prefer to use the method "visitLocalValues(...)"
     * @param localIndex : the local index, must be in range [0;numberOfSetLocalValues()[
     */
    const CT_Color& constColorAtLocalIndex(const size_t& localIndex) const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to count how many colors has been set.
     */
    size_t numberOfSetLocalValues() const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        to returns true if at least one color has been set.
     */
    bool hasLocalValues() const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        and call the visitor each time a color has been set.
     * @param v : a function that will receive for first parameter the global index of the color and for second parameter the value of the color. The
     *            visitor must returns false if the visit must be aborted or true to continue the visit.
     * @return False if the visitor has aborted the visit, true otherwise.
     */
    bool visitLocalValues(ColorVisitor v) const;

    /**
     * @brief This method will loop over the collection of global indexes (of points, edges or faces) set in the constructor
     *        and call the visitor each time a color has been set.
     * @param v : a function that will receive the global index of the color. The
     *            visitor must returns false if the visit must be aborted or true to continue the visit.
     * @return False if the visitor has aborted the visit, true otherwise.
     */
    bool visitLocalIndexesSet(IVisitor v) const;

    /**
     * @brief Returns the manager of the total color cloud
     */
    CT_AbstractXAttributeManager<CT_Color>* colorsManager() const;

    /**
     * @brief Copy colors of the source cloud for the destination cloud and let a modificator modify the new value to set
     * @param source : the source cloud
     * @param destination : the destination cloud (must be the same size as the source cloud)
     * @param modificator : a function that must modify the new value to set or nullptr to copy it only
     * @return false if destination has values already set ! true otherwise
     */
    bool copyAndModifyAttributesOfSForD(CT_CIR source, CT_CIR destination, CT_AbstractXAttributeManager<CT_Color>::AttributeModificator modificator = nullptr) { return colorsManager()->copyAndModifyAttributesOfSForD(source, destination, modificator); }

private:
    CT_CIR                                      m_cir;
    CT_AbstractXAttributeManager<CT_Color>*     m_manager;
};

template<typename InheritFrom>
CT_AttributesColor<InheritFrom>::CT_AttributesColor() :
    m_manager(nullptr)
{
}

template<typename InheritFrom>
CT_AttributesColor<InheritFrom>::CT_AttributesColor(CT_CIR cir,
                                                    CT_AbstractXAttributeManager<CT_Color>& manager) :

    m_cir(cir),
    m_manager(&manager)
{
}

template<typename InheritFrom>
const CT_Color& CT_AttributesColor<InheritFrom>::constColorAt(const size_t& globalIndex, bool* hasBeenSet) const
{
    return m_manager->tAt(globalIndex, hasBeenSet);
}

template<typename InheritFrom>
bool CT_AttributesColor<InheritFrom>::hasBeenSet(const size_t& globalIndex) const
{
    return m_manager->hasBeenSet(globalIndex);
}

template<typename InheritFrom>
size_t CT_AttributesColor<InheritFrom>::numberOfSetValues() const
{
    return m_manager->numberOfSetValues();
}

template<typename InheritFrom>
bool CT_AttributesColor<InheritFrom>::visitValues(CT_AttributesColor<InheritFrom>::ColorVisitor v) const
{
    return m_manager->visitValues(v);
}

template<typename InheritFrom>
bool CT_AttributesColor<InheritFrom>::visitAllIndexesSet(IVisitor v) const
{
    return m_manager->visitAllIndexesSet(v);
}

template<typename InheritFrom>
bool CT_AttributesColor<InheritFrom>::hasValues() const
{
    return !m_manager->isEmpty();
}

template<typename InheritFrom>
const CT_Color& CT_AttributesColor<InheritFrom>::constColorAtLocalIndex(const size_t& localIndex) const
{
    size_t count = 0;
    const CT_Color* color = nullptr;

    visitValues([&count, &localIndex, &color](const size_t&, const CT_Color& value) -> bool
    {
        if(count == localIndex)
        {
            color = &value;
            return false;
        }

        ++count;
        return true;
    });

    return *color;
}

template<typename InheritFrom>
size_t CT_AttributesColor<InheritFrom>::numberOfSetLocalValues() const
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
bool CT_AttributesColor<InheritFrom>::hasLocalValues() const
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
bool CT_AttributesColor<InheritFrom>::visitLocalValues(CT_AttributesColor<InheritFrom>::ColorVisitor v) const
{
    CT_AbstractCloudIndex* ci = m_cir->abstractCloudIndex();

    const size_t size = ci->size();
    bool hasBeenSet = false;

    for(size_t i=0; i<size; ++i)
    {
        const size_t globalIndex = ci->indexAt(i);
        const CT_Color& value = m_manager->tAt(globalIndex, &hasBeenSet);

        if(hasBeenSet)
        {
            if(!v(globalIndex, value))
                return false;
        }
    }

    return true;
}

template<typename InheritFrom>
bool CT_AttributesColor<InheritFrom>::visitLocalIndexesSet(IVisitor v) const
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
CT_AbstractXAttributeManager<CT_Color>* CT_AttributesColor<InheritFrom>::colorsManager() const
{
    return m_manager;
}

#endif // CT_ATTRIBUTESCOLOR_H

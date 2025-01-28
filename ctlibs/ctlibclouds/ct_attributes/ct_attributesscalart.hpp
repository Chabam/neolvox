#include "ct_attributes/ct_attributesscalart.h"

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::CT_AttributesScalarT() :
    m_manager(nullptr)
{
    CT_ScalarMinMaxManager<SCALAR>::InitMinMax(m_min, m_max);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::CT_AttributesScalarT(CT_CIR cir,
                                                                                CT_AbstractXAttributeManager<MANAGER_SCALAR>& manager,
                                                                                const SCALAR& min,
                                                                                const SCALAR& max) :
    m_cir(cir),
    m_min(min),
    m_max(max),
    m_manager(&manager)
{
    dynamic_cast<CT_ScalarMinMaxManager<MANAGER_SCALAR>*>(m_manager)->registerAttribute(this, m_min, m_max);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::CT_AttributesScalarT(CT_CIR cir,
                                                                                CT_AbstractXAttributeManager<MANAGER_SCALAR>& manager) :
    m_cir(cir),
    m_manager(&manager)
{
    CT_ScalarMinMaxManager<SCALAR>::InitMinMax(m_min, m_max);

    visitLocalValues([this](const size_t&, SCALAR value) -> bool
    {
        m_min = qMin(m_min, value);
        m_max = qMax(m_max, value);

        return true;
    });

    dynamic_cast<CT_ScalarMinMaxManager<MANAGER_SCALAR>*>(m_manager)->registerAttribute(this, m_min, m_max);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::~CT_AttributesScalarT()
{
    if(m_manager != nullptr)
        dynamic_cast<CT_ScalarMinMaxManager<MANAGER_SCALAR>*>(m_manager)->template unregisterAttribute<SCALAR>(this);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
double CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::scalarAsDoubleAt(const size_t& globalIndex, bool* hasBeenSet) const
{
    return double(convertScalarOfManagerToScalar(m_manager->tAt(globalIndex, hasBeenSet)));
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
SCALAR CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::scalarAt(const size_t& globalIndex, bool* hasBeenSet) const
{
    return convertScalarOfManagerToScalar(m_manager->tAt(globalIndex, hasBeenSet));
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
bool CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::hasBeenSet(const size_t& globalIndex) const
{
    return m_manager->hasBeenSet(globalIndex);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
size_t CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::numberOfSetValues() const
{
    return m_manager->numberOfSetValues();
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
bool CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::visitValuesAsDouble(DVisitor v) const
{
    return m_manager->visitValues([this, &v](const size_t& globalIndex, const MANAGER_SCALAR& value) -> bool
    {
        return v(globalIndex, double(convertScalarOfManagerToScalar(value)));
    });
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
bool CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::visitAllIndexesSet(IVisitor v) const
{
    return m_manager->visitAllIndexesSet(v);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
bool CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::hasValues() const
{
    return !m_manager->isEmpty();
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
double CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::minScalarAsDouble() const
{
    return double(minScalar());
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
double CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::maxScalarAsDouble() const
{
    return double(maxScalar());
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
SCALAR CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::minScalar() const
{
    return dynamic_cast<CT_ScalarMinMaxManager<MANAGER_SCALAR>*>(m_manager)->template min<SCALAR>(this);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
SCALAR CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::maxScalar() const
{
    return dynamic_cast<CT_ScalarMinMaxManager<MANAGER_SCALAR>*>(m_manager)->template max<SCALAR>(this);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
bool CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::visitValues(SVisitor v) const
{
    return m_manager->visitValues([this, &v](const size_t& globalIndex, const MANAGER_SCALAR& value) -> bool
    {
        return v(globalIndex, convertScalarOfManagerToScalar(value));
    });
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
double CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::scalarAsDoubleAtLocalIndex(const size_t& localIndex) const
{
    size_t count = 0;
    double scalarAsDouble = std::numeric_limits<double>::quiet_NaN();

    visitLocalValuesAsDouble([&count, &localIndex, &scalarAsDouble](const size_t&, double value) -> bool
    {
        if(count == localIndex)
        {
            scalarAsDouble = value;
            return false;
        }

        ++count;
        return true;
    });

    return scalarAsDouble;
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
SCALAR CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::scalarAtLocalIndex(const size_t& localIndex) const
{
    size_t count = 0;
    SCALAR scalarFounded = SCALAR();

    visitLocalValues([&count, &localIndex, &scalarFounded](const size_t&, SCALAR value) -> bool
    {
        if(count == localIndex)
        {
            scalarFounded = value;
            return false;
        }

        ++count;
        return true;
    });

    return scalarFounded;
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
size_t CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::numberOfSetLocalValues() const
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

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
bool CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::hasLocalValues() const
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

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
bool CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::visitLocalValuesAsDouble(CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::DVisitor v) const
{
    return visitLocalValues([&v](const size_t& globalIndex, SCALAR value) -> bool
    {
        return v(globalIndex, double(value));
    });
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
bool CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::visitLocalIndexesSet(IVisitor v) const
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

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
double CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::minLocalScalarAsDouble() const
{
    return double(m_min);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
double CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::maxLocalScalarAsDouble() const
{
    return double(m_max);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
SCALAR CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::minLocalScalar() const
{
    return m_min;
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
SCALAR CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::maxLocalScalar() const
{
    return m_max;
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
void CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::setLocalMinMax(const SCALAR& min, const SCALAR& max)
{
    m_min = min;
    m_max = max;

    dynamic_cast<CT_ScalarMinMaxManager<MANAGER_SCALAR>*>(m_manager)->updateAttribute(this, m_min, m_max);
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
bool CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::visitLocalValues(CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::SVisitor v) const
{
    CT_AbstractCloudIndex* ci = m_cir->abstractCloudIndex();

    const size_t size = ci->size();
    bool hasBeenSet = false;

    for(size_t i=0; i<size; ++i)
    {
        const size_t globalIndex = ci->indexAt(i);
        const MANAGER_SCALAR& value = m_manager->tAt(globalIndex, &hasBeenSet);

        if(hasBeenSet)
        {
            if(!v(globalIndex, convertScalarOfManagerToScalar(value)))
                return false;
        }
    }

    return true;
}

template<typename SCALAR, typename InheritFrom, typename MANAGER_SCALAR>
CT_AbstractXAttributeManager<MANAGER_SCALAR>* CT_AttributesScalarT<SCALAR, InheritFrom, MANAGER_SCALAR>::scalarsManager() const
{
    return m_manager;
}

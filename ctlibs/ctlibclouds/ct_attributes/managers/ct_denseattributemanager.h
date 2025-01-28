#ifndef CT_DENSEATTRIBUTEMANAGER_H
#define CT_DENSEATTRIBUTEMANAGER_H

#include <functional>

#include "ct_global/ct_cloudscontext.h"
#include "ct_global/ct_repositorymanager.h"

#include "ct_attributes/managers/abstract/ct_abstractxattributemanager.h"
#include "ct_attributes/setters/ct_denseattributesetter.h"
#include <QDebug>

template<typename T, typename TCIR, int syncWithT>
class CT_DenseAttributeManager : public CT_AbstractXAttributeManager<T>
{
    using SuperClass = CT_AbstractXAttributeManager<T>;

public:
    using Setter = CT_DenseAttributeSetter<T>;
    using SetterPtr = std::unique_ptr<Setter>;

    Setter createAttributesSetter(TCIR cir);
    SetterPtr createAttributesSetterPtr(TCIR cir);

    bool hasBeenSet(const size_t& globalIndex) const final;

    bool visitValues(typename CT_AbstractXAttributeManager<T>::Visitor v) const final;

    bool visitAllIndexesSet(typename CT_AbstractXAttributeManager<T>::IVisitor v) const final;

    size_t numberOfSetValues() const final;

    bool isEmpty() const final;

    const T& tAt(const size_t& globalIndex, bool* hasBeenSet = nullptr) const final;

    const T& tAtLocalIndex(const size_t& localIndex) const final;

    bool copyAndModifyAttributesOfSForD(CT_CIR source, CT_CIR destination, typename SuperClass::AttributeModificator modificator = nullptr) final;

    void createCollectionsIfNotCreated();

protected:
    typename CT_DenseAttributeSetter<T>::PtrAttributesCollectionType   mAttributes;
    typename CT_DenseAttributeSetter<T>::PtrAttributeBitCollectionType mBits;
    T                                                                  mDefaultValue;

    bool localIndexToGlobalIndex(const size_t& localIndex, size_t& globalIndex) const;
};

template<typename T, typename TCIR, int syncWithT>
typename CT_DenseAttributeManager<T, TCIR, syncWithT>::Setter CT_DenseAttributeManager<T, TCIR, syncWithT>::createAttributesSetter(TCIR cir)
{
    createCollectionsIfNotCreated();

    return CT_DenseAttributeSetter<T>(cir, mAttributes, mBits);
}

template<typename T, typename TCIR, int syncWithT>
typename CT_DenseAttributeManager<T, TCIR, syncWithT>::SetterPtr CT_DenseAttributeManager<T, TCIR, syncWithT>::createAttributesSetterPtr(TCIR cir)
{
    createCollectionsIfNotCreated();

    return std::make_unique<typename CT_DenseAttributeManager<T, TCIR, syncWithT>::Setter>(cir, mAttributes, mBits);
}

template<typename T, typename TCIR, int syncWithT>
bool CT_DenseAttributeManager<T, TCIR, syncWithT>::hasBeenSet(const size_t& globalIndex) const
{
    if(mAttributes.isNull())
        return false;

    return mBits->cloudT()->value(globalIndex) == true;
}

template<typename T, typename TCIR, int syncWithT>
bool CT_DenseAttributeManager<T, TCIR, syncWithT>::visitValues(typename CT_AbstractXAttributeManager<T>::Visitor v) const
{
    if(mAttributes.isNull())
        return true;

    const auto attributes = mAttributes->cloudT();
    const auto bits = mBits->cloudT();
    const size_t size = bits->size();

    for(size_t i=0; i<size; ++i)
    {
        if(bits->value(i) && !v(i, (*attributes)[i]))
            return false;
    }

    return true;
}

template<typename T, typename TCIR, int syncWithT>
bool CT_DenseAttributeManager<T, TCIR, syncWithT>::visitAllIndexesSet(typename CT_AbstractXAttributeManager<T>::IVisitor v) const
{
    if(mAttributes.isNull())
        return true;

    const auto bits = mBits->cloudT();
    const size_t size = bits->size();

    for(size_t i=0; i<size; ++i)
    {
        if(bits->value(i) && !v(i))
            return false;
    }

    return true;
}

template<typename T, typename TCIR, int syncWithT>
size_t CT_DenseAttributeManager<T, TCIR, syncWithT>::numberOfSetValues() const
{
    if(mAttributes.isNull())
        return false;

    const auto bits = mBits->cloudT();
    const size_t size = bits->size();

    size_t count = 0;
    for(size_t i=0; i<size; ++i)
    {
        if(bits->value(i))
            ++count;
    }

    return count;
}

template<typename T, typename TCIR, int syncWithT>
bool CT_DenseAttributeManager<T, TCIR, syncWithT>::isEmpty() const
{
    if(mAttributes.isNull())
        return true;

    const auto bits = mBits->cloudT();
    const size_t size = bits->size();

    for(size_t i=0; i<size; ++i)
    {
        if(bits->value(i))
            return false;
    }

    return true;
}

template<typename T, typename TCIR, int syncWithT>
const T& CT_DenseAttributeManager<T, TCIR, syncWithT>::tAt(const size_t& globalIndex, bool* hasBeenSet) const
{
    if(mAttributes.isNull())
    {
        if(hasBeenSet != nullptr)
            *hasBeenSet = false;

        return mDefaultValue;
    }

    const auto attributes = mAttributes->cloudT();

    if(hasBeenSet != nullptr)
        (*hasBeenSet) = mBits->cloudT()->value(globalIndex);

    return (*attributes)[globalIndex];
}

template<typename T, typename TCIR, int syncWithT>
const T& CT_DenseAttributeManager<T, TCIR, syncWithT>::tAtLocalIndex(const size_t& localIndex) const
{
    size_t globalIndex = 0;

    if(!localIndexToGlobalIndex(localIndex, globalIndex))
        return mDefaultValue;

    const auto attributes = mAttributes->cloudT();
    return (*attributes)[globalIndex];
}

template<typename T, typename TCIR, int syncWithT>
bool CT_DenseAttributeManager<T, TCIR, syncWithT>::copyAndModifyAttributesOfSForD(CT_CIR source, CT_CIR destination, typename SuperClass::AttributeModificator modificator)
{
    if(mAttributes.isNull()
//            || source.dynamicCast<typename TCIR::element_type>().isNull() // correction AP 09/11/2022
            || source.isNull()
            || destination.isNull()
            || destination.dynamicCast<typename TCIR::element_type>().isNull())
        return false;

    auto sourceCI = source->abstractCloudIndex();
    auto destCI = destination->abstractCloudIndex();

    const auto sourceSize = sourceCI->size();
    const auto destSize = destCI->size();

    if(destSize == 0)
        return true;

    if(hasBeenSet(destCI->indexAt(0)))
        return false;

    auto setter = createAttributesSetter(destination.dynamicCast<typename TCIR::element_type>());

    if(modificator == nullptr)
    {
        for(unsigned long i=0; i<sourceSize && i<destSize; ++i)
        {
            setter.setValueWithGlobalIndex(destCI->indexAt(i), tAt(sourceCI->indexAt(i)));
        }
    }
    else
    {
        T destValue;

        for(unsigned long i=0; i<sourceSize && i<destSize; ++i)
        {
            const auto sourceGlobalIndex = sourceCI->indexAt(i);
            const T& sourceValue = tAt(sourceCI->indexAt(i));
            const auto destinationGlobalIndex = destCI->indexAt(i);

            const int ret = modificator(sourceGlobalIndex, sourceValue, destinationGlobalIndex, destValue);

            if(ret > 0)
                setter.setValueWithGlobalIndex(destinationGlobalIndex, destValue);
            else if(ret < 0)
                return true;
        }
    }

    return true;
}

template<typename T, typename TCIR, int syncWithT>
void CT_DenseAttributeManager<T, TCIR, syncWithT>::createCollectionsIfNotCreated()
{
    if(!mAttributes.isNull())
        return;

    mAttributes = PS_REPOSITORY->createNewCloudT<typename CT_DenseAttributeSetter<T>::RegisteredAttributesCollectionType, typename CT_DenseAttributeSetter<T>::AttributesCollectionType>(CT_Repository::SyncCloudWith(syncWithT));
    mBits = PS_REPOSITORY->createNewCloudT<typename CT_DenseAttributeSetter<T>::RegisteredAttributeBitCollectionType, typename CT_DenseAttributeSetter<T>::AttributeBitCollectionType>(CT_Repository::SyncCloudWith(syncWithT));
}

template<typename T, typename TCIR, int syncWithT>
bool CT_DenseAttributeManager<T, TCIR, syncWithT>::localIndexToGlobalIndex(const size_t& localIndex, size_t& globalIndex) const
{
    if(mAttributes.isNull())
        return false;

    const auto bits = mBits->cloudT();
    const size_t size = bits->size();

    size_t count = 0;
    for(size_t i=0; i<size; ++i)
    {
        if(bits->value(i))
        {
            if(count == localIndex)
            {
                globalIndex = i;
                return true;
            }

            ++count;
        }
    }

    return false;
}

#endif // CT_DENSEATTRIBUTEMANAGER_H


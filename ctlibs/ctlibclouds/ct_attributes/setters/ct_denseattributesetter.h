#ifndef CT_DENSEATTRIBUTESETTER_H
#define CT_DENSEATTRIBUTESETTER_H

#include "ct_defines.h"

#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregistered.h"
#include "ct_cloudindex/abstract/ct_abstractcloudindex.h"

#include "ct_cloud/registered/ct_stdcloudregisteredt.h"
#include "ct_cloud/ct_standardcloudstdvectort.h"
#include "ct_cloud/ct_bitcloud.h"

/**
 * @brief The CT_DenseAttributeSetter class is the setter of attributes
 *        to use when the user has created a cloud (points, faces or edges) registered (with
 *        the size defined).
 */
template<typename T>
class CT_DenseAttributeSetter
{
public:
    using AttributesCollectionType = CT_StandardCloudStdVectorT<T>;
    using RegisteredAttributesCollectionType = CT_StdCloudRegisteredT<AttributesCollectionType>;
    using PtrAttributesCollectionType = QSharedPointer<RegisteredAttributesCollectionType>;

    using AttributeBitCollectionType = CT_BitCloud;
    using RegisteredAttributeBitCollectionType = CT_StdCloudRegisteredT<AttributeBitCollectionType>;
    using PtrAttributeBitCollectionType = QSharedPointer<RegisteredAttributeBitCollectionType>;

    CT_DenseAttributeSetter(CT_CIR cir,
                             PtrAttributesCollectionType attributes,
                             PtrAttributeBitCollectionType exists);

    /**
     * @brief Set the value of the attribute with a global index.
     * @param globalIndex : the global index of the attribute.
     * @param value : the value to set
     */
    void setValueWithGlobalIndex(const size_t& globalIndex, const T& value);

    /**
     * @brief Set the value of the attribute with a local index.
     * @param localIndex : the local index of the attribute. Must be between [0; cir.size()[
     * @param value : the value to set
     */
    void setValueWithLocalIndex(const size_t& localIndex, const T& value);

    /**
     * @brief Set the value of the attribute with a global index and with help of a stream (QDataStream or QTextStream per example).
     * @param globalIndex : the global index of the attribute.
     * @param stream : the stream to use to get the value
     */
    template<typename Stream>
    void setValueWithStreamWithGlobalIndex(const size_t& globalIndex, Stream& stream);

    /**
     * @brief Set the value of the attribute with a local index and with help of a stream (QDataStream or QTextStream per example).
     * @param localIndex : the local index of the attribute. Must be between [0; cir.size()[
     * @param stream : the stream to use to get the value
     */
    template<typename Stream>
    void setValueWithStreamWithLocalIndex(const size_t& localIndex, Stream& stream);

    /**
     * @brief Unset the attribute at specified global index. This will not remove the attribute but set
     *        in the bits cloud that the attribute has not been set.
     *
     * @param globalIndex : the global index of the attribute.
     */
    void unSetValueWithGlobalIndex(const size_t& globalIndex);

    /**
     * @brief Unset the attribute at specified local index. This will not remove the attribute but set
     *        in the bits cloud that the attribute has not been set.
     *
     * @param localIndex : the local index of the attribute. Must be between [0; cir.size()[
     */
    void unSetValueWithLocalIndex(const size_t& localIndex);

private:
    CT_CIR                                  mCloudRegistered;
    PtrAttributesCollectionType             mAttributes;
    PtrAttributeBitCollectionType           mBits;
};

template<typename T>
CT_DenseAttributeSetter<T>::CT_DenseAttributeSetter(CT_CIR cir,
                                                      CT_DenseAttributeSetter::PtrAttributesCollectionType attributes,
                                                      CT_DenseAttributeSetter::PtrAttributeBitCollectionType exists) :
    mCloudRegistered(cir),
    mAttributes(attributes),
    mBits(exists)
{
}

template<typename T>
void CT_DenseAttributeSetter<T>::setValueWithGlobalIndex(const size_t& globalIndex, const T& value)
{
    (*mAttributes->cloudT())[globalIndex] = value;
    (*mBits->cloudT()).set(globalIndex, true);
}

template<typename T>
void CT_DenseAttributeSetter<T>::setValueWithLocalIndex(const size_t& localIndex, const T& value)
{
    setValueWithGlobalIndex(mCloudRegistered->abstractCloudIndex()->indexAt(localIndex), value);
}

template<typename T>
template<typename Stream>
void CT_DenseAttributeSetter<T>::setValueWithStreamWithGlobalIndex(const size_t& globalIndex, Stream& stream)
{
    stream >> (*mAttributes->cloudT())[globalIndex];
    (*mBits->cloudT()).set(globalIndex, true);
}

template<typename T>
template<typename Stream>
void CT_DenseAttributeSetter<T>::setValueWithStreamWithLocalIndex(const size_t& localIndex, Stream& stream)
{
    setValueWithStreamWithGlobalIndex(mCloudRegistered->abstractCloudIndex()->indexAt(localIndex), stream);
}

template<typename T>
void CT_DenseAttributeSetter<T>::unSetValueWithGlobalIndex(const size_t& globalIndex)
{
    (*mBits->cloudT()).set(globalIndex, false);
}

template<typename T>
void CT_DenseAttributeSetter<T>::unSetValueWithLocalIndex(const size_t& localIndex)
{
    unSetValueWithGlobalIndex(mCloudRegistered->abstractCloudIndex()->indexAt(localIndex));
}

#endif // CT_DENSEATTRIBUTESETTER_H

